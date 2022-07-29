#include "Session.h"

#include <iostream>

Session::Session(Socket* sock, const InternetAddress* addr, const uint16_t MTU, const int64_t GUID)
		: socket(sock), MTU(MTU), GUID(GUID) {
	std::scoped_lock<std::mutex> lock(this->mutex);
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

Session::State Session::getState() const {
	return this->state;
}

bool Session::isConnected() const {
	return this->state == State::CONNECTED;
}

void Session::update() {
	std::scoped_lock<std::mutex> lock(this->mutex);
	if (time(0) - lastUpdate > 5000)
			this->disconnect("timeout");
	
	if (!this->layer->ACKQueue.sequenceNumbers.empty()) {
		auto buffer = std::make_unique<PacketSerializer>();
		this->layer->ACKQueue.encode(buffer.get());
		this->layer->ACKQueue.sequenceNumbers.clear();
		this->sendPacket(buffer.get());
		Logger::getInstance()->notice("ACK sended!");
		this->lastUpdate = time(0);
	}
	if (!this->layer->NACKQueue.sequenceNumbers.empty()) {
		auto buffer = std::make_unique<PacketSerializer>();
		this->layer->NACKQueue.encode(buffer.get());
		this->layer->NACKQueue.sequenceNumbers.clear();
		this->sendPacket(buffer.get());
		Logger::getInstance()->notice("NACK sended!");
		this->lastUpdate = time(0);
	}
	if (!this->layer->updateQueue.empty()) {
		this->layer->sendDatagram(this->layer->updateQueue);
		this->layer->updateQueue.clear();
		this->lastUpdate = time(0);
	}
}

void Session::disconnect(std::string reason) {
	Logger::getInstance()->notice(LogMessage() << "Session {" << this->addr->toString() << "} disconnected. Reason: " << reason);
	this->state = State::DISCONNECTED;
	this->socket->getSessionManager()->closeSession(this->addr.get());
}

void Session::receivePacket(PacketSerializer* buffer) {
	std::scoped_lock<std::mutex> lock(this->mutex);
	this->lastUpdate = time(0);
	
	uint8_t pid = buffer->getBuffer()[0];
	if (pid == ID::ACK) {
		Logger::getInstance()->notice("Handle ACK");
		Acknowledge<ID::ACK> ack;
		ack.decode(buffer);
		
		for (uint32_t seqNumber : ack.sequenceNumbers) {
			if (this->layer->recoveryQueue[seqNumber].get() != nullptr) {
				this->layer->recoveryQueue[seqNumber].reset(nullptr);
				this->layer->recoveryQueue.erase(seqNumber);
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
		auto datagram = std::make_unique<Datagram>();
		
		/*for (int i = 0; i < buffer->getSize(); i++) {
			std::cout << (int) buffer->getBuffer()[i] << " ";
		}
		std::cout << std::endl;*/
		
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
				datagram->packets.push_back(std::move(this->layer->recvOrderedPackets[i][this->layer->recvOrderedIndex[i]]));
				this->layer->recvOrderedIndex[i]++;
				this->layer->recvOrderedPackets[i].erase(minOrderedIndex(i));
				
				countLateOrderedPackets++;
			}
		}
		
		Logger::getInstance()->notice(LogMessage() << "Session {" << this->addr->toString() << "} receive " << datagram->packets.size() << " packets, discard " << discardPacket << " packets, add " << countLateOrderedPackets << " packets");
		
		if (!datagram->packets.empty()) {
			auto f = ThreadManager::getInstance()->addTask([this, d = std::move(datagram)] {
				this->handleRawPacket(d.get());
			});
		}
	} else {
		throw SocketException() << "Undefined packet ID: " << pid;
	}
}

void Session::handleRawPacket(Datagram* datagram) {
	for (auto packet = datagram->packets.begin(); packet != datagram->packets.end(); packet++) {
		auto buffer = std::make_unique<PacketSerializer>((*packet)->buffer.getBuffer(), (*packet)->length);
		uint8_t pid = buffer->getBuffer()[0];
		if (pid == ID::CONNECTION_REQUEST) {
			Logger::getInstance()->notice("Handle ConnectionRequest packet");
			auto recvPacket = std::make_unique<ConnectionRequest>();
			recvPacket->decode(buffer.get());
			buffer->clear();
			
			auto sendPacket = std::make_unique<ConnectionRequestAccepted>();
			sendPacket->clientAddress = this->addr.get();
			sendPacket->requestTime = recvPacket->time;
			sendPacket->time = this->socket->getTime();
			sendPacket->encode(buffer.get());
			
			//std::scoped_lock<std::mutex> lock(this->mutex);
			this->layer->sendEncapsulated(buffer.get(), Reliability::UNRELIABLE, 0, QueuePriority::IMMEDIATE);
		} else if (pid == ID::NEW_INCOMING_CONNECTION) {
			Logger::getInstance()->notice("Handle NewIncomingConnection packet");
			buffer->clear();
			
			this->state = State::CONNECTED;
			
			/*auto sendPing = std::make_unique<ConnectedPing>();
			sendPing->pingTime = this->socket->getTime();
			sendPing->encode(buffer.get());
			
			this->layer->sendEncapsulated(buffer.get(), Reliability::UNRELIABLE, 0);*/
		} else if (pid == ID::CONNECTED_PING) {
			Logger::getInstance()->notice("Handle ConnectedPing packet");
			auto recvPacket = std::make_unique<ConnectedPing>();
			recvPacket->decode(buffer.get());
			buffer->clear();
			
			auto sendPacket = std::make_unique<ConnectedPong>();
			sendPacket->pingTime = recvPacket->pingTime;
			sendPacket->pongTime = this->socket->getTime();
			sendPacket->encode(buffer.get());
			
			this->layer->sendEncapsulated(buffer.get(), Reliability::UNRELIABLE, 0);
		} else if (pid == ID::CONNECTED_PONG) {
			Logger::getInstance()->notice("Handle ConnectedPong packet");
			auto recvPacket = std::make_unique<ConnectedPong>();
			recvPacket->decode(buffer.get());
			
			uint64_t currentTime = this->socket->getTime();
			if (currentTime < recvPacket->pingTime) {
				Logger::getInstance()->debug(LogMessage() << "Received invalid pong: timestamp is in the future by " << recvPacket->pingTime - currentTime << " ms");
			}
		} else {
			Logger::getInstance()->warning(LogMessage() << "Undefined packet ID: " << (int) pid << " size: " << buffer->getSize());
			for (int i = 0; i < buffer->getSize(); i++) {
				std::cout << (int) buffer->getBuffer()[i] << " ";
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
