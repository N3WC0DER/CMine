#pragma once
#include <string>

#include "Packet.h"

class UnconnectedPing : public Packet {
public:
	static const uint8_t PID = ID::UNCONNECTED_PING;
	
	int64_t pingTime;
	int64_t userGUID;
	
	void decodeHeader(PacketSerializer* out);
	void decodePayload(PacketSerializer* out);
	
	void encodeHeader(PacketSerializer* in);
	void encodePayload(PacketSerializer* in);
};