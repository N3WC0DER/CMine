#include "Packet.h"

Packet::Packet(int recvBytes, char* payload) {
	this->size = recvBytes;
	
	this->payload = new char[recvBytes];
	for (int i = 0; i < recvBytes; i++)
			this->payload[i] = payload[i];
}

int Packet::getSize() const {
	return this->size;
}

char* Packet::decodePayload() {
	return this->payload;
}