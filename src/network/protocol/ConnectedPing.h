#pragma once
#include "network/protocol/Packet.h"

class ConnectedPing : public Packet<ConnectedPing> {
public:
	static const uint8_t PID = ID::CONNECTED_PING;
	
	uint64_t pingTime;
	
	void decodePayload(PacketSerializer* out) {
		this->pingTime = out->readLong();
	}
	void encodePayload(PacketSerializer* in) {
		in->putLong(this->pingTime);
	}
};