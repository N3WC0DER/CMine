#pragma once
#include <vector>

#include "utils/Logger.h"
#include "network/protocol/Packet.h"
#include "network/protocol/EncapsulatedPacket.h"

class Datagram : public Packet<Datagram> {
public:
	static const uint8_t PID = 0x80;
	
	uint32_t sequenceNumber;
	
	std::vector<std::unique_ptr<EncapsulatedPacket>> packets;
	
	void decodePayload(PacketSerializer* out) {
		this->sequenceNumber = out->readLTriad();
		while(!out->feof()) {
			auto temp = std::make_unique<EncapsulatedPacket>();
			temp->decode(out);
			this->packets.push_back(std::move(temp));
		}
	}
	void encodePayload(PacketSerializer* in) {
		in->putLTriad(this->sequenceNumber);
		for (auto packet = this->packets.begin(); packet != this->packets.end(); packet++) {
			(*packet)->encode(in);
		}
	}
	
	size_t getTotalLength() const {
		size_t length = 1 + 3; // id + sequenceNumber
		for (auto packet = this->packets.begin(); packet != this->packets.end(); packet++) {
			length += (*packet)->getLength();
		}
		return length;
	}
};