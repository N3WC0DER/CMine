#include "BinaryStream.h"

#include <iostream>

BinaryStream::BinaryStream()
		: buffer(nullptr), size(0), capacity(0), position(0), endian(Endian::BENDIAN) {}

BinaryStream::BinaryStream(const uint8_t* buffer, const int size, Endian endian)
		: size(size), capacity(size), position(0), endian(endian) {
	this->buffer = new uint8_t[size];
	for (int i = 0; i < size; i++)
			this->buffer[i] = buffer[i];
}

BinaryStream::BinaryStream(const BinaryStream& stream)
		: BinaryStream(stream.getBuffer(), stream.getSize(), stream.getEndian()) {}

BinaryStream::~BinaryStream() {
	this->clear();
}

uint8_t* BinaryStream::getBuffer() const {
	return this->buffer;
}

int BinaryStream::getSize() const {
	return this->size;
}

Endian BinaryStream::getEndian() const {
	return this->endian;
}

int BinaryStream::getCapacity() const {
	return this->capacity;
}

void BinaryStream::changeEndian(){
	if (this->endian == Endian::BENDIAN) {
		this->endian = Endian::LENDIAN;
	} else {
		this->endian = Endian::BENDIAN;
	}
}

void BinaryStream::clear() {
	if (this->buffer != nullptr)
			delete this->buffer;
	this->buffer = nullptr;
	this->size = 0;
	this->position = 0;
	this->capacity = 0;
}

bool BinaryStream::feof() const {
	return this->position == this->size;
}

//----------------------------- READ -----------------------------

uint8_t* BinaryStream::read(const size_t bytes) const {
	uint8_t* data = new uint8_t[bytes];
	if (this->endian == Endian::BENDIAN) {
		for (size_t i = 0; i < bytes; i++) {
			data[i] = this->readByte();
		}
	} else {
		for (size_t i = bytes; i > 0; i--) {
			data[i] = this->readByte();
		}
	}
	return data;
}

uint8_t BinaryStream::readByte() const {
	if (this->position >= this->size)
			throw Exception("Binary stream out of range!");
	
	return buffer[position++];
}

int64_t BinaryStream::readLong() const {
	int64_t value = 0;
	
	if (this->endian == Endian::BENDIAN) {
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
	
	return value;
}

int16_t BinaryStream::readShort() const {
	return static_cast<int16_t>(this->readUShort());
}

uint16_t BinaryStream::readUShort() const {
	uint16_t value = 0;
	
	if (this->endian == Endian::BENDIAN) {
		value |= this->readByte() << 8;
		value |= this->readByte() << 0;
	} else {
		value |= this->readByte() << 0;
		value |= this->readByte() << 8;
	}
	return value;
}

int32_t BinaryStream::readInt() const {
	return static_cast<int32_t>(readUInt());
}

uint32_t BinaryStream::readUInt() const {
	uint32_t value = 0;
	if (this->endian == Endian::BENDIAN) {
		value |= this->readByte() << 24;
		value |= this->readByte() << 16;
		value |= this->readByte() << 8;
		value |= this->readByte() << 0;
	} else {
		value |= this->readByte() << 0;
		value |= this->readByte() << 8;
		value |= this->readByte() << 16;
		value |= this->readByte() << 24;
	}
	return value;
}

bool BinaryStream::readBoolean() const {
	return this->readByte() == 1 ? true : false;
}

//----------------------------- PUT -----------------------------

// TODO: Set a size limit
void BinaryStream::put(const uint8_t* data, const size_t bytes) {
	if (this->capacity < this->size + bytes) {
		uint8_t temp[size+bytes];
		
		// Copy the old array and delete it
		if (this->buffer != nullptr) {
			for (int i = 0; i < this->size; i++)
					temp[i] = this->buffer[i];
			
			delete this->buffer;
			this->buffer = nullptr;
		}
		
		// Copy data from parameter
		for (int i = this->size; i < this->size + bytes; i++)
				temp[i] = data[i-size];
		
		// Create an array and copy all elements from a temporary array
		this->size += bytes;
		this->capacity = this->size + BLOCK;
		this->buffer = new uint8_t[this->capacity];
		
		for (int i = 0; i < this->size; i++)
				this->buffer[i] = temp[i];
	} else {
		for (int i = this->size; i < this->size + bytes; i++) {
			this->buffer[i] = data[i-size];
		}
		this->size += bytes;
	}
}

void BinaryStream::putByte(const uint8_t value) {
	if (this->capacity < this->size + 1) {
		uint8_t temp[this->size+BLOCK];
		temp[this->size] = value;
		
		if (this->buffer != nullptr) {
			for (int i = 0; i < this->size; i++)
					temp[i] = this->buffer[i];
			
			delete this->buffer;
			this->buffer = nullptr;
		}
		
		this->size++;
		this->capacity = this->size + BLOCK;
		this->buffer = new uint8_t[this->capacity];
		
		for (int i = 0; i < this->size; i++)
				this->buffer[i] = temp[i];
	} else {
		this->buffer[this->size++] = value;
	}
}

void BinaryStream::putLong(const int64_t value) {
	if (endian == Endian::BENDIAN) {
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

void BinaryStream::putShort(const int16_t value) {
	this->putUShort(value);
}

void BinaryStream::putUShort(const uint16_t value) {
	if (endian == Endian::BENDIAN) {
		this->putByte(value >> 8);
		this->putByte(value >> 0);
	} else {
		this->putByte(value >> 0);
		this->putByte(value >> 8);
	}
}

void BinaryStream::putInt(const int32_t value) {
	this->putUInt(value);
}

void BinaryStream::putUInt(const uint32_t value) {
	if (endian == Endian::BENDIAN) {
		this->putByte(value >> 24);
		this->putByte(value >> 16);
		this->putByte(value >> 8);
		this->putByte(value >> 0);
	} else {
		this->putByte(value >> 0);
		this->putByte(value >> 8);
		this->putByte(value >> 16);
		this->putByte(value >> 24);
	}
}

void BinaryStream::putBoolean(const bool value) {
	this->putByte(value == true ? 1 : 0);
}