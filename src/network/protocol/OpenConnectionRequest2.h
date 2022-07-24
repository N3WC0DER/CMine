#pragma once

#include "network/protocol/Packet.h"
#include "network/utils/InternetAddress.h"

class OpenConnectionRequest2 : public Packet<OpenConnectionRequest2> {
public:
	static const uint8_t PID = ID::OPEN_CONNECTION_REQUEST_2;
	
	uint8_t protocolVersion;
	InternetAddress* serverAddr;
	uint16_t MTU;
	int64_t userGUID;
	
	void decodePayload(PacketSerializer* out) {
		out->readMagic();
		this->serverAddr = out->readAddress();
		this->MTU = out->readUShort();
		this->userGUID = out->readLong();
	}
	void encodePayload(PacketSerializer* in) {
		in->putMagic();
		in->putAddress(this->serverAddr);
		in->putUShort(this->MTU);
		in->putLong(this->userGUID);
	}
};