#pragma once
#include <vector>
#include <algorithm>

#include "network/protocol/Packet.h"

template <int T>
class Acknowledge : public Packet<Acknowledge<T>> {
public:
	static const uint8_t PID = T;
	
	std::vector<uint32_t> sequenceNumbers;
	
	void decodePayload(PacketSerializer* out) {
		int16_t count = out->readShort();
		this->sequenceNumbers.clear();
		
		for (int i = 0; i < count; i++) {
			if (out->readBoolean() == true) {
				this->sequenceNumbers.push_back(out->readLTriad());
			} else {
				uint32_t start = out->readLTriad();
				uint32_t end = out->readLTriad();
				
				if ((end - start) > 512)
						end = start + 512;
				
				for (int j = start; j < end; j++) {
					this->sequenceNumbers.push_back(j);
				}
			}
		}
	}
	void encodePayload(PacketSerializer* in) {
		PacketSerializer payload;
		std::sort(this->sequenceNumbers.begin(), this->sequenceNumbers.end());
		uint32_t count = this->sequenceNumbers.size();
		uint32_t records = 0;
		
		if (count > 0) {
			size_t pointer = 1;
			uint32_t start = this->sequenceNumbers[0];
			uint32_t last = this->sequenceNumbers[0];
			
			while (pointer < count) {
				uint32_t current = this->sequenceNumbers[pointer++];
				size_t diff = current - last;
				if (diff == 1) {
					last = current;
				} else if (diff > 1) {
					if (start == last) {
						payload.putBoolean(true); // Record type single
						payload.putLTriad(start);
						start = last = current;
					} else {
						payload.putBoolean(false); // Record type range
						payload.putLTriad(start);
						payload.putLTriad(last);
						start = last = current;
					}
					records++;
				}
			}
			
			if (start == last) {
				payload.putBoolean(true);
				payload.putLTriad(start);
			} else {
				payload.putBoolean(false);
				payload.putLTriad(start);
				payload.putLTriad(last);
			}
			records++;
		}
		
		in->putShort(records);
		in->put(payload.getBuffer(), payload.getSize());
	}
};