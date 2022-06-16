#include "PacketSerializer.h"

PacketSerializer::PacketSerializer(char* buffer, int recvBytes)
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
		uint16_t port = this->readShort();
		return new InternetAddress(addr, port);
	} else if (version == 6) {
		// TODO
		throw std::runtime_error("IPv6 not supported!");
	} else {
		throw std::runtime_error("Invalid version " + std::to_string(version));
	}
}

uint32_t PacketSerializer::readLEndian() {
	uint32_t value = 0;
	
	if (this->endian == Endians::BENDIAN) {
		value |= this->readByte() << 16;
		value |= this->readByte() << 8;
		value |= this->readByte() << 0;
	} else {
		value |= this->readByte() << 0;
		value |= this->readByte() << 8;
		value |= this->readByte() << 16;
	}
	return value;
}

void PacketSerializer::putString(std::string value) {
	unsigned short length = value.size();
	
	for (int i = 0; i < length; i++)
			this->putByte(value[i]);
}

void PacketSerializer::putAddress(InternetAddress* addr) {
	this->putByte(addr->getVersion());
	if (addr->getVersion() == 4){
		uint8_t temp;
		for (int i = 0; i < addr->getAddress().size(); i++) {
			if (addr->getAddress()[i] == '.') {
				this->putByte(temp);
				temp = 0;
			} else {
				temp *= 10;
				temp += (int) addr->getAddress()[i];
				if (i == addr->getAddress().size()-1)
						this->putByte(temp);
			}
		}
	} else if (addr->getVersion() == 6) {
		// TODO
		throw std::runtime_error("IPv6 not supported");
	} else {
		throw std::runtime_error("Invalid version " + std::to_string(addr->getVersion()));
	}
}

void PacketSerializer::putLEndian(uint32_t value) {
	value &= 0xFFFFFF;
	
	if (endian == Endians::BENDIAN) {
		this->putByte(value >> 16);
		this->putByte(value >> 8);
		this->putByte(value >> 0);
	} else {
		this->putByte(value >> 0);
		this->putByte(value >> 8);
		this->putByte(value >> 16);
	}
}