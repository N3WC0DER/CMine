#pragma once
#include <string>

#include "network/protocol/Packet.h"

class UnconnectedPong : public Packet<UnconnectedPong> {
public:
	static const uint8_t PID = ID::UNCONNECTED_PONG;
	
	int64_t pingTime;
	int64_t serverGUID;
	std::string serverID;
	
	void decodePayload(PacketSerializer* out) {
		this->pingTime = out->readLong();
		this->serverGUID = out->readLong();
		out->readMagic();
		this->serverID = out->readString();
	}
	void encodePayload(PacketSerializer* in) {
		in->putLong(this->pingTime);
		in->putLong(this->serverGUID);
		in->putMagic();
		in->putString(serverID);
	}
};