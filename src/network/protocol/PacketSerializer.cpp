#include "PacketSerializer.h"

#include <iostream>

PacketSerializer::PacketSerializer() 
		: BinaryStream() {}

PacketSerializer::PacketSerializer(uint8_t* buffer, int recvBytes)
		: BinaryStream(buffer, recvBytes) {}

std::string PacketSerializer::readString() {
	unsigned short length = this->readUShort();
	
	std::string value;
	
	for (int i = 0; i < length; i++)
			value += this->readByte();
	
	return value;
}

InternetAddress* PacketSerializer::readAddress() {
	int version = this->readByte();
	
	if (version == 4) {
		std::string addr = std::to_string(this->readByte()) + "." + std::to_string(this->readByte()) + "." + std::to_string(this->readByte()) + "." + std::to_string(this->readByte());
		uint16_t port = this->readUShort();
		return new InternetAddress(addr, port);
	} else if (version == 6) {
		// TODO
		throw std::runtime_error("IPv6 not supported!");
	} else {
		throw std::runtime_error("Invalid version " + std::to_string(version));
	}
}

uint8_t* PacketSerializer::readLTriad() {
	uint8_t* value = new uint8_t[3];
	
	value[0] = this->readByte();
	value[1] = this->readByte();
	value[2] = this->readByte();
	return value;
}

void PacketSerializer::readMagic() {
	this->position += 16;
}

void PacketSerializer::putString(std::string value) {
	unsigned short length = value.size();
	this->putUShort(length);
	
	for (int i = 0; i < length; i++)
			this->putByte(static_cast<uint8_t>(value[i]));
}

void PacketSerializer::putAddress(InternetAddress* addr) {
	this->putByte(addr->getVersion());
	
	if (addr->getVersion() == 4){
		std::string temp;
		uint8_t byte;
		for (int i = 0; i < addr->getAddress().size(); i++) {
			if (addr->getAddress()[i] == '.') {
				byte = std::stoi(temp);
				this->putByte(byte);
				
				byte = 0;
				temp.clear();
				
			} else {
				temp += static_cast<uint8_t>(addr->getAddress()[i]);
			
				if (i == addr->getAddress().size()-1) {
					byte = std::stoi(temp);
					this->putByte(byte);
					byte = 0;
					temp.clear();
				}
			}
		}
		
		this->putUShort(addr->getPort());
	} else if (addr->getVersion() == 6) {
		// TODO
		throw std::runtime_error("IPv6 not supported");
	} else {
		throw std::runtime_error("Invalid version " + std::to_string(addr->getVersion()));
	}
}

void PacketSerializer::putLTriad(uint8_t* value) {
	value[0] &= 0xFF;
	value[1] &= 0xFF;
	value[2] &= 0xFF;
	
	this->putByte(value[0]);
	this->putByte(value[1]);
	this->putByte(value[2]);
}

void PacketSerializer::putMagic() {
	for (int i = 0; i < 16; i++) {
		this->putByte(magic[i]);
	}
}