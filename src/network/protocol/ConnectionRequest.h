#pragma once
#include "network/protocol/Packet.h"

class ConnectionRequest : public Packet<ConnectionRequest> {
public:
	static const uint8_t PID = ID::CONNECTION_REQUEST;
	
	int64_t GUID;
	uint64_t time;
	
	void decodePayload(PacketSerializer* out) {
		this->GUID = out->readLong();
		this->time = out->readLong();
	}
	void encodePayload(PacketSerializer* in) {
		in->putLong(this->GUID);
		in->putLong(this->time);
	}
};