#include "UnconnectedPing.h"

void UnconnectedPing::decodeHeader(PacketSerializer* out) {
	out->readByte(); //PID
}

void UnconnectedPing::decodePayload(PacketSerializer* out) {
	this->pingTime = out->readLong();
	out->readMagic();
	this->userGUID = out->readLong();
}

void UnconnectedPing::encodeHeader(PacketSerializer* in) {
	in->putByte(this->PID);
}

void UnconnectedPing::encodePayload(PacketSerializer* in) {
	in->putLong(this->pingTime);
	in->putMagic();
	in->putLong(this->userGUID);
}