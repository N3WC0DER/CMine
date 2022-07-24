#pragma once
#include "network/protocol/Packet.h"
#include "network/utils/InternetAddress.h"

class OpenConnectionReply2 : public Packet<OpenConnectionReply2> {
public:
	static const uint8_t PID = ID::OPEN_CONNECTION_REPLY_2;
	
	uint64_t serverGUID;
	InternetAddress* clientAddr;
	uint16_t MTU;
	bool encryption;
	
	void decodePayload(PacketSerializer* out) {
		out->readMagic();
		this->serverGUID = out->readLong();
		this->clientAddr = out->readAddress();
		this->MTU = out->readUShort();
		this->encryption = out->readBoolean();
	}
	void encodePayload(PacketSerializer* in) {
		in->putMagic();
		in->putLong(this->serverGUID);
		in->putAddress(this->clientAddr);
		in->putUShort(this->MTU);
		in->putBoolean(this->encryption);
	}
};