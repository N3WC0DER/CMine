#pragma once
#include "../utils/InternetAddress.h"
#include "../../utils/BinaryStream.h"

class PacketSerializer : public BinaryStream{
public:
	PacketSerializer(char* buffer, int recvBytes);
	
	std::string readString();
	InternetAddress* readAddress();
	uint32_t readLEndian();
	
	void putString(std::string str);
	void putAddress(InternetAddress* addr);
	void putLEndian(uint32_t lend);
};