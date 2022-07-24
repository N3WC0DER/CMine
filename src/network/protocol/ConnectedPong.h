#pragma once
#include "network/protocol/Packet.h"

class ConnectedPong : public Packet<ConnectedPong> {
public:
	static const uint8_t PID = ID::CONNECTED_PONG;
	
	uint64_t pingTime;
	uint64_t pongTime;
	
	void decodePayload(PacketSerializer* out) {
		this->pingTime = out->readLong();
		this->pongTime = out->readLong();
	}
	void encodePayload(PacketSerializer* in) {
		in->putLong(this->pingTime);
		in->putLong(this->pongTime);
	}
};