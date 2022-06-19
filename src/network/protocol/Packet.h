#pragma once

#include "MessageIdentifier.h"
#include "PacketSerializer.h"

class Packet {
private:
protected:
	virtual void decodeHeader(PacketSerializer* out) = 0;
	virtual void decodePayload(PacketSerializer* out) = 0;
	
	virtual void encodeHeader(PacketSerializer* in) = 0;
	virtual void encodePayload(PacketSerializer* in) = 0;

public:
	static const uint8_t PID = ID::UNKNOWN;
	
	void decode(PacketSerializer* out);
	
	void encode(PacketSerializer* in);
};