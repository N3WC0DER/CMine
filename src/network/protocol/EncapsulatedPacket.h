#pragma once
#include "network/protocol/PacketSerializer.h"
#include "network/protocol/PacketReliability.h"

struct SplitInfo {
	uint32_t compoundSize = 0;
	uint16_t compoundID;
	uint32_t compoundIndex;
};

class EncapsulatedPacket {
public:
	uint8_t reliability;
	uint16_t length;
	uint32_t messageIndex = 0xFF000000;
	uint32_t sequencedIndex = 0xFF000000;
	
	uint32_t orderedIndex = 0xFF000000;
	uint8_t orderChannel = 0xFF;
	
	bool hasSplit;
	SplitInfo splitInfo;
	
	BinaryStream buffer;
	
	void decode(PacketSerializer* out) {
		uint8_t flags = out->readByte();
		this->reliability = (flags & (0b111 << 5)) >> 5;
		this->hasSplit = (flags & 0b00010000) == 1 ? true : false;
		
		this->length = out->readUShort() >> 3;
		
		if (PacketReliability::isReliable(this->reliability)) {
			this->messageIndex = out->readLTriad();
		}
		
		if (PacketReliability::isSequenced(this->reliability)) {
			this->sequencedIndex = out->readLTriad();
		}
		
		if (PacketReliability::isSequencedOrOrdered(this->reliability)) {
			this->orderedIndex = out->readLTriad();
			this->orderChannel = out->readByte();
		}
		
		if (this->hasSplit) {
			this->splitInfo.compoundSize = out->readUInt();
			this->splitInfo.compoundID = out->readUShort();
			this->splitInfo.compoundIndex = out->readUInt();
		}
		
		this->buffer.clear();
		this->buffer.put(out->read(this->length), this->length);
	}
	
	void encode(PacketSerializer* in) {
		if (this->hasSplit) {
			in->putByte((this->reliability << 5) ^ 0b00010000);
		} else {
			in->putByte(this->reliability << 5);
		}
		
		in->putShort(this->length << 3);
		
		if (PacketReliability::isReliable(this->reliability)) {
			in->putLTriad(this->messageIndex);
		}
		
		if (PacketReliability::isSequenced(this->reliability)) {
			in->putLTriad(this->sequencedIndex);
		}
		
		if (PacketReliability::isSequencedOrOrdered(this->reliability)) {
			in->putLTriad(this->orderedIndex);
			in->putByte(this->orderChannel);
		}
		
		if (this->hasSplit) {
			in->putUInt(this->splitInfo.compoundSize);
			in->putUShort(this->splitInfo.compoundID);
			in->putUInt(this->splitInfo.compoundIndex);
		}
		
		in->put(this->buffer.read(this->length), this->length);
	}
	
	size_t getLength() const {
		return 1 + //reliability 
					 2 + //length
					 (PacketReliability::isReliable(this->reliability) ? 3 : 0) + //messageIndex
					 (PacketReliability::isSequenced(this->reliability) ? 7 : 0) + //sequencedIndex + orderedIndex + orderChannel
					 (PacketReliability::isOrdered(this->reliability) ? 4 : 0) + //orderedIndex + orderChannel
					 (this->hasSplit ? 10 : 0) + //split 
					 this->length;
	}
};