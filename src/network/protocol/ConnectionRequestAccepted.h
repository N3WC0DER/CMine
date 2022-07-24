#pragma once
#include "network/protocol/Packet.h"
#include "network/utils/InternetAddress.h"

class ConnectionRequestAccepted : public Packet<ConnectionRequestAccepted> {
private:
	uint16_t systemIndex = 0;
	InternetAddress internalID = InternetAddress("255.255.255.255", 19132);

public:
	static const uint8_t PID = ID::CONNECTION_REQUEST_ACCEPTED;
	
	InternetAddress* clientAddress;
	uint64_t requestTime;
	uint64_t time;
	
	void decodePayload(PacketSerializer* out) {
		this->clientAddress = out->readAddress();
		/*this->systemIndex = */out->readShort();
		for (int i = 0; i < 10; i++)
				/*this->internalID = */out->readAddress();
				
		this->requestTime = out->readLong();
		this->time = out->readLong();
	}
	void encodePayload(PacketSerializer* in) {
		in->putAddress(this->clientAddress);
		in->putShort(this->systemIndex);
		for (int i = 0; i < 10; i++)
				in->putAddress(&(this->internalID));
		
		in->putLong(this->requestTime);
		in->putLong(this->time);
	}
};