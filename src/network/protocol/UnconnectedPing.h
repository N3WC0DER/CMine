#pragma once
#include <string>

#include "network/protocol/Packet.h"

class UnconnectedPing : public Packet<UnconnectedPing> {
public:
	static const uint8_t PID = ID::UNCONNECTED_PING;
	
	int64_t pingTime;
	int64_t userGUID;
	
	void decodePayload(PacketSerializer* out) {
		this->pingTime = out->readLong();
		out->readMagic();
		this->userGUID = out->readLong();
	}
	void encodePayload(PacketSerializer* in) {
		in->putLong(this->pingTime);
		in->putMagic();
		in->putLong(this->userGUID);
	}
};