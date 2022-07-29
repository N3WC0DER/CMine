#pragma once

#include "network/protocol/MessageIdentifier.h"
#include "network/protocol/PacketSerializer.h"

template <typename T>
class Packet {
private:
protected:
	void decodeHeader(PacketSerializer* out) {
		out->readByte(); //PID
	}
	
	virtual void decodePayload(PacketSerializer* out) = 0;
	
	void encodeHeader(PacketSerializer* in) {
		in->putByte(T::PID);
	}
	
	virtual void encodePayload(PacketSerializer* in) = 0;

public:
	void decode(PacketSerializer* out) {
		this->decodeHeader(out);
		this->decodePayload(out);
	}
	
	void encode(PacketSerializer* in) {
		this->encodeHeader(in);
		this->encodePayload(in);
	}
	
	virtual ~Packet() = default;
};