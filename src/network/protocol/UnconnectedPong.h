#pragma once
#include <string>

#include "Packet.h"

class UnconnectedPong : public Packet {
public:
	static const uint8_t PID = ID::UNCONNECTED_PONG;
	
	int64_t pingTime;
	int64_t serverGUID;
	std::string serverID;
	
	void decodeHeader(PacketSerializer* out);
	void decodePayload(PacketSerializer* out);
	
	void encodeHeader(PacketSerializer* in);
	void encodePayload(PacketSerializer* in);
};