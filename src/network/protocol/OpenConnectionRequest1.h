#pragma once

#include "network/protocol/Packet.h"

class OpenConnectionRequest1 : public Packet<OpenConnectionRequest1> {
public:
	static const uint8_t PID = ID::OPEN_CONNECTION_REQUEST_1;
	
	uint8_t protocolVersion;
	uint16_t MTU;
	
	void decodePayload(PacketSerializer* out) {
		out->readMagic();
		this->protocolVersion = out->readByte();
		this->MTU = out->getSize() - 18;
	}
	void encodePayload(PacketSerializer* in) {
		in->putMagic();
		in->putByte(this->protocolVersion);
		//in->putUShort(this->MTU);
	}
};