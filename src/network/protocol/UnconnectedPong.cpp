#include "UnconnectedPong.h"

void UnconnectedPong::decodeHeader(PacketSerializer* out) {
	out->readByte(); //PID
}

void UnconnectedPong::decodePayload(PacketSerializer* out) {
	this->pingTime = out->readLong();
	this->serverGUID = out->readLong();
	out->readMagic();
	this->serverID = out->readString();
}

void UnconnectedPong::encodeHeader(PacketSerializer* in) {
	in->putByte(this->PID);
}

void UnconnectedPong::encodePayload(PacketSerializer* in) {
	in->putLong(this->pingTime);
	in->putLong(this->serverGUID);
	in->putMagic();
	in->putString(serverID);
}