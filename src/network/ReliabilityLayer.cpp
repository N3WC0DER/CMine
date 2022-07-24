#include "ReliabilityLayer.h"

ReliabilityLayer::ReliabilityLayer(std::function<void(const PacketSerializer*)> sendPacket) 
		: sendPacket(sendPacket) {}

size_t ReliabilityLayer::handleDatagram(Datagram* datagram) {
	this->ACKQueue.sequenceNumbers.push_back(datagram->sequenceNumber);
	
	if (this->recvSequenceNumber < datagram->sequenceNumber) {
		this->lastRecvSequenceNumber = this->recvSequenceNumber;
		this->recvSequenceNumber = datagram->sequenceNumber;
		
		if (this->recvSequenceNumber - this->lastRecvSequenceNumber > 1) {
			for (uint32_t seqNumber = this->lastRecvSequenceNumber + 1; seqNumber < this->recvSequenceNumber; seqNumber++) {
				this->NACKQueue.sequenceNumbers.push_back(seqNumber);
			}
		}
		
	} else if (this->recvSequenceNumber > datagram->sequenceNumber) {
		for (size_t i = 0; i < this->NACKQueue.sequenceNumbers.size(); i++) {
			if (datagram->sequenceNumber == this->NACKQueue.sequenceNumbers[i]) {
				this->NACKQueue.sequenceNumbers.erase(this->NACKQueue.sequenceNumbers.begin() + i);
			}
		}
	}
	// TODO: Обрабатывать пакет или нет в зависимости от порядкового номера и надежности пакета
	size_t countDiscardPackets = datagram->packets.size();
	this->handleEncapsulated(datagram);
	
	return countDiscardPackets - datagram->packets.size();
}

void ReliabilityLayer::handleEncapsulated(Datagram* datagram) {
	std::vector<std::unique_ptr<EncapsulatedPacket>> handledPackets;
	for (auto packet = datagram->packets.begin(); packet != datagram->packets.end(); packet++) {
		if (PacketReliability::isReliable((*packet)->reliability)) {
			if (find(this->recvReliableIndex.begin(), this->recvReliableIndex.end(), (*packet)->messageIndex) != this->recvReliableIndex.end()) {
				Logger::getInstance()->warning(LogMessage() << "Duplicate message index (reliable packet): " << (*packet)->messageIndex);
				if (PacketReliability::isOrdered(packet->reliability))
						Logger::getInstance()->warning(LogMessage() << "Order channel: " << (*packet)->orderChannel);
				Logger::getInstance()->warning(LogMessage() << "Has split: " << ((*packet)->hasSplit ? "true" : "false"));
				Logger::getInstance()->warning(LogMessage() << "ID: " << (int) (*packet)->buffer.getBuffer()[0]);
				Logger::getInstance()->warning(LogMessage() << "Length: " << (*packet)->length);
				(*packet).reset(nullptr);
				continue;
			}
			this->recvReliableIndex.push_back((*packet)->messageIndex);
		}
		
		if ((*packet)->hasSplit && !this->handleSplit((*packet).get())) {
			(*packet).reset(nullptr));
			continue;
		}
		
		if (PacketReliability::isSequencedOrOrdered((*packet)->reliability) && (*packet)->orderChannel > MAX_ORDER_CHANNELS) {
			Logger::getInstance()->warning(LogMessage() << "Bad order channel: " << (*packet)->orderChannel);
			(*packet).reset(nullptr);
			continue;
		}
		
		if (PacketReliability::isSequenced((*packet)->reliability)) {
			if ((*packet)->sequencedIndex < this->recvSequencedIndex[(*packet)->orderChannel]) {
				Logger::getInstance()->warning(LogMessage() << "Old sequenced packet (sequence index " << (*packet)->sequencedIndex << ")");
				(*packet).reset(nullptr);
				continue;
			}
			
			this->recvSequencedIndex[(*packet)->orderChannel] = (*packet)->sequencedIndex + 1;
		} else if (PacketReliability::isOrdered((*packet)->reliability)) {
			if ((*packet)->orderedIndex == this->recvOrderedIndex[(*packet)->orderChannel]) {
				this->recvOrderedIndex[(*packet)->orderChannel] = (*packet)->orderedIndex + 1;
			} else if ((*packet)->orderedIndex > this->recvOrderedIndex[(*packet)->orderChannel]) {
				auto pck = std::make_unique<EncapsulatedPacket>;
				pck->buffer = (*packet)->buffer;
				pck->length = (*packet)->length;
				this->recvOrderedPackets[(*packet)->orderChannel][(*packet)->orderedIndex] = pck;
				(*packet).reset(nullptr);
				continue;
			} else {
				(*packet).reset(nullptr);
				continue;
			}
		}
		auto pck = std::make_unique<EncapsulatedPacket>;
		pck->buffer = (*packet)->buffer;
		pck->length = (*packet)->length;
		handledPackets.push_back(pck);
		(*packet).reset(nullptr);
	}
	datagram->packets = handledPackets;
}

bool ReliabilityLayer::handleSplit(EncapsulatedPacket* packet) {
	if (!packet->hasSplit)
			return true;
	
	uint32_t totalParts = packet->splitInfo.compoundSize;
	uint32_t partIndex = packet->splitInfo.compoundIndex;
	uint16_t splitID = packet->splitInfo.compoundID;
	
	if (totalParts >= MAX_SPLIT_PART_COUNT || partIndex >= totalParts) {
		Logger::getInstance()->warning(LogMessage() << "Invalid split packet part (total parts " << totalParts << ", part index " << partIndex << ")");
		return false;
	}
	
	if (this->splitPackets[splitID].empty()) {
		if (this->splitPackets.size() > MAX_SPLIT_PACKETS) {
			Logger::getInstance()->warning(LogMessage() << "Ignored split packet part because reached concurrent split packet limit of " << MAX_SPLIT_PACKETS);
			return false;
		}
	}
	
	auto temp = std::make_unique<EncapsulatedPacket>;
	temp->buffer = packet->buffer;
	temp->length = packet->length;
	this->splitPackets[splitID][partIndex] = temp.move();
	
	if (this->splitPackets[splitID].size() != totalParts) {
		return false;
	}
	
	int totalLength = 0;
	std::vector<std::unique_ptr<EncapsulatedPacket>> parts;
	for (size_t i = 0; i < this->splitPackets[splitID].size(); i++) {
		totalLength += this->splitPackets[splitID][i]->length;
		parts[i] = this->splitPackets[splitID][i].move();
	}
	
	packet->length = totalLength;
	packet->hasSplit = false;
	
	for (auto part = parts.begin(); part != parts.end(); part++) {
		packet->buffer.put((*part)->buffer.read((*part)->length), (*part)->length);
		(*part).reset(nullptr);
	}
	
	this->splitPackets[splitID].clear();
	
	return true;
}

void ReliabilityLayer::sendEncapsulated(PacketSerializer* buffer, Reliability reliability, uint8_t orderChannel, QueuePriority priority) {
	auto packet = std::make_unique<EncapsulatedPacket>;
	packet->buffer = *buffer;
	packet->length = buffer->getSize();
	
	if (orderChannel > MAX_ORDER_CHANNELS) {
		Logger::getInstance()->warning(LogMessage() << "Order channel out of range: " << orderChannel);
		packet.reset(nullptr);
		return;
	}
	
	packet->reliability = reliability;
	packet->orderChannel = orderChannel;
	
	if (PacketReliability::isOrdered(reliability)) {
		packet->orderedIndex = this->sendOrderedIndex[orderChannel]++ % 0xF000000;
	}
	
	if (PacketReliability::isSequenced(reliability)) {
		packet->orderedIndex = this->sendOrderedIndex[orderChannel];
		packet->sequencedIndex = this->sendSequencedIndex[orderChannel]++ % 0xF000000;
	}
	
	//IP header size (20 bytes) + UDP header size (8 bytes) + RakNet weird (8 bytes) + datagram header size (4 bytes) + max encapsulated packet header size (20 bytes)
	size_t maxSize = this->sessionMTU - 60;
	if (packet->length > maxSize) {
		uint16_t splitID = this->splitID++ % 0xF0000;
		uint32_t partIndex = 0;
		std::unique_ptr<PacketSerializer> temp = nullptr;
		for (size_t i = packet->length; i > 0; i -= maxSize) {
			if (i >= maxSize) {
				temp = std::make_unique<PacketSerializer>(buffer->read(maxSize), maxSize);
			} else {
				temp = std::make_unique<PacketSerializer>(buffer->read(i), i);
			}
			auto pck = std::make_unique<EncapsulatedPacket>;
			pck->buffer = *temp;
			pck->length = temp->getSize();
			
			pck->reliability = packet->reliability;
			
			if (PacketReliability::isReliable(pck->reliability)) {
				pck->messageIndex = this->sendReliableIndex++ % 0xF000000;
			}
			
			pck->sequencedIndex = packet->sequencedIndex;
			pck->orderChannel = packet->orderChannel;
			pck->orderedIndex = packet->orderedIndex;
			
			pck->hasSplit = true;
			pck->splitInfo.compoundID = splitID;
			pck->splitInfo.compoundSize = (int) packet->length / maxSize + (packet->length % maxSize > 0 ? 1 : 0);
			pck->splitInfo.compoundIndex = partIndex++;
			
			this->sendDatagram(pck.get());
		}
	} else {
		if (priority == QueuePriority::IMMEDIATE) {
			this->sendDatagram(packet.get());
		} else if (priority == QueuePriority::UPDATE) {
			this->updateQueue.push_back(packet.move());
		} else if (priority == QueuePriority::FULLQ) {
			size_t length = packet->getLength();
			for (auto pck = this->normalQueue.begin(); pck != this->normalQueue.end(); pck++) {
				length += (*pck)->getLength();
			}
			
			if (length >= this->sessionMTU - 40) {
				this->sendDatagram(this->normalQueue);
				this->normalQueue.clear();
			}
			
			this->normalQueue.push_back(packet.move());
		} else {
			Logger::getInstance()->warning(LogMessage() << "Bad priority: " << (int) priority);
		}
	}
}

void ReliabilityLayer::sendDatagram(std::vector<std::unique_ptr<EncapsulatedPacket>>& packets) {
	if (packets.empty())
			return;
	
	Datagram datagram;
	datagram.sequenceNumber = this->sendSequenceNumber++;
	
	while (datagram.getTotalLength() + (*packets.front())->getLength() + 36 < this->sessionMTU) {
		datagram.packets.push_back(packets.front());
		packets.erase(packets.begin());
	}
	
	auto buffer = std::make_unique<PacketSerializer>;
	datagram.encode(buffer.get());
	
	auto recoveryPacket = std::make_unique<PacketSerializer>(buffer);
	this->recoveryQueue[datagram.sequenceNumber] = recoveryPacket.move();
	
	(this->sendPacket)(buffer.get());
}

void ReliabilityLayer::sendDatagram(EncapsulatedPacket* packet) {
	std::vector<std::unique_ptr<EncapsulatedPacket>> packets;
	packets.push_back(packet);
	this->sendDatagram(packets);
}