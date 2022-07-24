#include "Session.h"

#include <iostream>

Session::Session(Socket* sock, InternetAddress* addr, uint16_t MTU, int64_t GUID)
		: socket(sock), MTU(MTU), GUID(GUID) {
	this->addr = std::make_unique<InternetAddress>(*addr);
	using std::placeholders::_1;
	this->layer = std::make_unique<ReliabilityLayer>(std::bind(&Session::sendPacket, this, _1));
	this->layer->sessionMTU = this->MTU;
	
	this->lastUpdate = time(0);
}

InternetAddress* Session::getAddress() const {
	return this->addr.get();
}

uint16_t Session::getMTU() const {
	return this->MTU;
}

int64_t Session::getGUID() const {
	return this->GUID;
}

void Session::update() {
	if (time(0) - lastUpdate > 5000)
			this->disconnect();
	
	/*if (!this->layer->ACKQueue.sequenceNumbers.empty()) {
		PacketSerializer buffer;
		this->layer->ACKQueue.decode(&buffer);
		this->sendPacket(&buffer);
	}*/
	if (!this->layer->NACKQueue.sequenceNumbers.empty()) {
		auto buffer = std::make_unique<PacketSerializer>;
		this->layer->NACKQueue.decode(&buffer);
		this->sendPacket(buffer);
		this->lastUpdate = time(0);
	}
	if (!this->layer->updateQueue.empty()) {
		this->layer->sendDatagram(this->layer->updateQueue);
		this->layer->updateQueue.clear();
		this->lastUpdate = time(0);
	}
}

void Session::disconnect() {
	this->socket->getSessionManager()->closeSession(this->addr.get());
}

void Session::receivePacket(PacketSerializer* buffer) {
	this->lastUpdate = time(0);
	
	uint8_t pid = buffer->getBuffer()[0];
	if (pid == ID::ACK) {
		Logger::getInstance()->notice("Handle ACK");
		Acknowledge<ID::ACK> ack;
		ack.decode(buffer);
		
		for (uint32_t seqNumber : ack.sequenceNumbers) {
			if (this->layer->recoveryQueue[seqNumber].get() != nullptr) {
				this->layer->recoveryQueue[seqNumber].reset(nullptr);
			}
		}
	} else if (pid == ID::NACK) {
		Logger::getInstance()->notice("Handle NACK");
		Acknowledge<ID::NACK> nack;
		nack.decode(buffer);
		
		for (uint32_t seqNumber : nack.sequenceNumbers) {
			if (this->layer->recoveryQueue[seqNumber].get() != nullptr) {
				this->sendPacket(this->layer->recoveryQueue[seqNumber].get());
			}
		}
	} else if (pid >= 0x80 && pid <= 0x8F) {
		auto datagram = std::make_unique<Datagram>;
		datagram->decode(buffer);
		
		size_t discardPacket = this->layer->handleDatagram(datagram.get());
    size_t countLateOrderedPackets = 0;
		
		auto minOrderedIndex = [this] (uint8_t orderChannel) {
			size_t minimum = 0;
			for (auto it = this->layer->recvOrderedPackets[orderChannel].begin(); it != this->layer->recvOrderedPackets[orderChannel].end(); it++) {
				if (it->first < minimum)
						minimum = it->first;
			}
			return minimum;
		};
		
		for (int i = 0; i < this->layer->recvOrderedPackets.size(); i++) {
			if (this->layer->recvOrderedPackets[i].empty())
					continue;
			
			// Эти пакеты были добавлены в очередь, но обработались в будущем через основную дейтаграмму
			while (minOrderedIndex(i) < this->layer->recvOrderedIndex[i])
					this->layer->recvOrderedPackets[i].erase(minOrderedIndex(i));
			
			// Добавляем пакеты в дейтаграмму, пока не дойдем до очередного пропуска
			while (minOrderedIndex(i) == this->layer->recvOrderedIndex[i]) {
				datagram->packets.push_back(this->layer->recvOrderedPackets[i][this->layer->recvOrderedIndex[i]].move());
				this->layer->recvOrderedIndex[i]++;
				this->layer->recvOrderedPackets[i].erase(minOrderedIndex(i));
				
				countLateOrderedPackets++;
			}
		}
		
		Logger::getInstance()->notice(LogMessage() << "Session {" << this->addr->toString() << "} receive " << datagram->packets.size() << " packets, discard " << discardPacket << " packets, add " << countLateOrderedPackets << " packets");
		
		if (!datagram->packets.empty()) {
			std::thread th([this, &datagram] {
				this->handleRawPacket(datagram.move());
			});
			th.detach();
		}
	} else {
		throw SocketException() << "Undefined packet ID: " << pid;
	}
}

void Session::handleRawPacket(Datagram* datagram) {
	for (auto packet : datagram->packets) {
		auto buffer = std::make_unique<PacketSerializer>(packet->buffer.getBuffer(), packet->length);
		//packet->encode(buffer);
		uint8_t pid = buffer->getBuffer()[0];
		if (pid == ID::CONNECTION_REQUEST) {
			Logger::getInstance()->notice("Handle ConnectionRequest packet");
			auto recvPacket = std::make_unique<ConnectionRequest>;
			recvPacket->decode(buffer);
			buffer->clear();
			uint64_t time = recvPacket->time;
			
			auto sendPacket = std::make_unique<ConnectionRequestAccepted>;
			sendPacket->clientAddress = this->addr.get();
			sendPacket->requestTime = time;
			sendPacket->time = this->socket->getTime();
			sendPacket->encode(buffer);
			
			this->layer->sendEncapsulated(buffer, Reliability::UNRELIABLE, 0, QueuePriority::IMMEDIATE);
		} else if (pid == ID::NEW_INCOMING_CONNECTION) {
			Logger::getInstance()->notice("Handle NewIncomingConnection packet");
		} else {
			Logger::getInstance()->warning(LogMessage() << "Undefined packet ID: " << (int) pid << " size: " << packet->length);
			for (int i = 0; i < packet->buffer.getSize(); i++) {
				std::cout << (int) packet->buffer.getBuffer()[i] << " ";
			}
			std::cout << std::endl;
		}
	}
}

void Session::sendPacket(const PacketSerializer* packet) {
	this->socket->send(packet->getBuffer(), packet->getSize(), this->addr.get());
}

/*void Session::addPacketToQueue(PacketSerializer* buffer, uint8_t reliability, uint8_t orderChannel, QueuePriority priority) {
	EncapsulatedPacket packet;
	packet->reliability = reliability;
	packet->orderChannel = orderChannel;
	
	if (priority == QueuePriority::IMMEDIATE) {
		
	} else if (priority == QueuePriority::UPDATE) {
		this->updateQueue.push(packet);
	} else if (priority == QueuePriority::FULLQ) {
		this->normalQueue.push(packet);
		if (this->normalQueue.size() > this->MTU) {
			for (PacketSerializer* pck : this->normalQueue) {
				this->sendPacket(pck);
				this->recoveryQueue[]
			}
		}
	}
}*/
