#include "Packet.h"

void Packet::decode(PacketSerializer* out) {
	this->decodeHeader(out);
	this->decodePayload(out);
}

void Packet::encode(PacketSerializer* in) {
	this->encodeHeader(in);
	this->encodePayload(in);
}