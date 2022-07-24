#pragma once
#include "network/protocol/Packet.h"

class IncompatibleProtocol : public Packet<IncompatibleProtocol> {
public:
	static const uint8_t PID = ID::INCOMPATIBLE_PROTOCOL;
	
	uint8_t protocolVersion;
	int64_t serverGUID;
	
	void decodePayload(PacketSerializer* out) {
		this->protocolVersion = out->readByte();
		out->readMagic();
		this->serverGUID = out->readLong();
	}
	void encodePayload(PacketSerializer* in) {
		in->putByte(this->protocolVersion);
		in->putMagic();
		in->putLong(this->serverGUID);
	}
};