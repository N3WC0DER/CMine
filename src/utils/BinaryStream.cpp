#include "BinaryStream.h"

BinaryStream::BinaryStream()
		: size(0), position(0), endian(Endians::LENDIAN) {}

BinaryStream::BinaryStream(char *str, int size, Endians endian)
		: buffer(str), size(size), position(0), endian(endian) {}

uint8_t BinaryStream::readByte() {
	if (this->position >= this->size)
			throw std::runtime_error("Binary stream out of range!");
	
	return static_cast<uint8_t>(buffer[position++]);
}

int64_t BinaryStream::readLong() {
	int64_t value = 0;
	
	if (this->endian == Endians::BENDIAN) {
		value |= static_cast<unsigned long>(this->readByte()) << 56;
		value |= static_cast<unsigned long>(this->readByte()) << 48;
		value |= static_cast<unsigned long>(this->readByte()) << 40;
		value |= static_cast<unsigned long>(this->readByte()) << 32;
		value |= static_cast<unsigned long>(this->readByte()) << 24;
		value |= static_cast<unsigned long>(this->readByte()) << 16;
		value |= static_cast<unsigned long>(this->readByte()) << 8;
		value |= static_cast<unsigned long>(this->readByte()) << 0;
	} else {
		value |= static_cast<unsigned long>(this->readByte()) << 0;
		value |= static_cast<unsigned long>(this->readByte()) << 8;
		value |= static_cast<unsigned long>(this->readByte()) << 16;
		value |= static_cast<unsigned long>(this->readByte()) << 24;
		value |= static_cast<unsigned long>(this->readByte()) << 32;
		value |= static_cast<unsigned long>(this->readByte()) << 40;
		value |= static_cast<unsigned long>(this->readByte()) << 48;
		value |= static_cast<unsigned long>(this->readByte()) << 56;
	}
	
	if (value >> 63 == 1) {
		value = -(value ^ 0xFFFFFFFFFFFFFFFF);
	}
	
	return value;
}

short BinaryStream::readShort() {
	short value = static_cast<short>(this->readUShort());
	
	if (value >> 15 == 1) {
		value = -(value ^ 0xFFFF);
	}
	return value;
}

unsigned short BinaryStream::readUShort() {
	unsigned short value = 0;
	
	if (this->endian == Endians::BENDIAN) {
		value |= this->readByte() << 8;
		value |= this->readByte() << 0;
	} else {
		value |= this->readByte() << 0;
		value |= this->readByte() << 8;
	}
	return value;
}

bool BinaryStream::readBoolean() {
	return this->readByte() == 1 ? true : false;
}

void BinaryStream::putByte(uint8_t value) {
	char *temp = this->buffer;
	this->size++;
	this->buffer = new char[this->size];
	
	this->buffer = temp + value;
	delete temp;
	temp = nullptr;
}

void BinaryStream::putLong(int64_t value) {
	if (endian == Endians::BENDIAN) {
		this->putByte(value >> 56);
		this->putByte(value >> 48);
		this->putByte(value >> 40);
		this->putByte(value >> 32);
		this->putByte(value >> 24);
		this->putByte(value >> 16);
		this->putByte(value >> 8);
		this->putByte(value >> 0);
	} else {
		this->putByte(value >> 0);
		this->putByte(value >> 8);
		this->putByte(value >> 16);
		this->putByte(value >> 24);
		this->putByte(value >> 32);
		this->putByte(value >> 40);
		this->putByte(value >> 48);
		this->putByte(value >> 56);
	}
}

void BinaryStream::putShort(short value) {
	if (endian == Endians::BENDIAN) {
		this->putByte(value >> 8);
		this->putByte(value >> 0);
	} else {
		this->putByte(value >> 0);
		this->putByte(value >> 8);
	}
}

void BinaryStream::putUShort(unsigned short value) {
	if (endian == Endians::BENDIAN) {
		this->putByte(value >> 8);
		this->putByte(value >> 0);
	} else {
		this->putByte(value >> 0);
		this->putByte(value >> 8);
	}
}

void BinaryStream::putBoolean(bool value) {
	this->putByte(value == true ? 1 : 0);
}