#include <string>

#include "Socket.h"
#include "protocol/Packet.h"
#include "../utils/Mutex.h"

#include "PacketHandler.h"

PacketHandler::PacketHandler(Socket* socket)
	: socket(socket) {}

void PacketHandler::handlePacket() {
	while (true) {
		Mutex::stopHandler.lock();
		if (this->stopped){
			Mutex::stopHandler.unlock();
			break;
		}
		Mutex::stopHandler.unlock();
		
		Packet* packet = this->socket->getPacket();
		
		if (packet == nullptr)
				continue;
		
		int size = packet->getSize();
		char* tempPayload = packet->decodePayload();
		std::string payload;
		
		for (int i = 0; i < size; i++) {
			payload += tempPayload[i];
		}
		
		this->socket->getServer()->getLogger()->info("Packet size: " + std::to_string(size));
		this->socket->getServer()->getLogger()->info("Payload: " + payload);
	}
}

void PacketHandler::close() {
	this->socket->getServer()->getLogger()->info("Handler stopped..");
	Mutex::stopHandler.lock();
	this->stopped = true;
	Mutex::stopHandler.unlock();
}