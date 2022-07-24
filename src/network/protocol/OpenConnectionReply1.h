#pragma once
#include "network/protocol/Packet.h"

class OpenConnectionReply1 : public Packet<OpenConnectionReply1> {
public:
	static const uint8_t PID = ID::OPEN_CONNECTION_REPLY_1;
	
	uint64_t serverGUID;
	bool security;
	uint16_t MTU;
	
	void decodePayload(PacketSerializer* out) {
		out->readMagic();
		this->serverGUID = out->readLong();
		this->security = out->readBoolean();
		this->MTU = out->readUShort();
	}
	void encodePayload(PacketSerializer* in) {
		in->putMagic();
		in->putLong(this->serverGUID);
		in->putBoolean(this->security);
		in->putUShort(this->MTU);
	}
};