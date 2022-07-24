#include "BinaryStream.h"

#include <iostream>

BinaryStream::BinaryStream()
		: size(0), position(0), endian(Endians::BENDIAN) {}

BinaryStream::BinaryStream(const uint8_t* buffer, const int size, Endians endian)
		: size(size), position(0), endian(endian) {
	this->buffer = new uint8_t[size];
	for (int i = 0; i < size; i++)
			this->buffer[i] = buffer[i];
}

BinaryStream::BinaryStream(const BinaryStream& stream)
		: BinaryStream(stream.getBuffer(), stream.getSize()) {}

BinaryStream::~BinaryStream() {
	delete buffer;
	this->buffer = nullptr;
}

uint8_t* BinaryStream::getBuffer() const {
	return this->buffer;
}

int BinaryStream::getSize() const {
	return this->size;
}

void BinaryStream::clear() {
	this->buffer = nullptr;
	this->size = 0;
	this->position = 0;
}

bool BinaryStream::feof() const {
	return this->position == this->size;
}

//----------------------------- READ -----------------------------

uint8_t* BinaryStream::read(const size_t bytes) {
	uint8_t* data = new uint8_t[bytes];
	if (this->endian == Endians::BENDIAN) {
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

uint8_t BinaryStream::readByte() {
	if (this->position >= this->size)
			throw Exception("Binary stream out of range!");
	
	return buffer[position++];
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
	
	return value;
}

int16_t BinaryStream::readShort() {
	return static_cast<int16_t>(this->readUShort());
}

uint16_t BinaryStream::readUShort() {
	uint16_t value = 0;
	
	if (this->endian == Endians::BENDIAN) {
		value |= this->readByte() << 8;
		value |= this->readByte() << 0;
	} else {
		value |= this->readByte() << 0;
		value |= this->readByte() << 8;
	}
	return value;
}

int32_t BinaryStream::readInt() {
	return static_cast<int32_t>(readUInt());
}

uint32_t BinaryStream::readUInt() {
	uint32_t value = 0;
	if (this->endian == Endians::BENDIAN) {
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

bool BinaryStream::readBoolean() {
	return this->readByte() == 1 ? true : false;
}

//----------------------------- PUT -----------------------------

void BinaryStream::put(const uint8_t* data, const size_t bytes) {
	uint8_t temp[size+bytes];
	
	// Копируем старый массив и удаляем его
	if (this->buffer != nullptr) {
		for (int i = 0; i < this->size; i++)
				temp[i] = this->buffer[i];
		
		delete this->buffer;
		this->buffer = nullptr;
	}
	
	// Копируем данные из параметра
	if (this->endian == Endians::BENDIAN) {
		for (int i = this->size; i < bytes; i++) {
			temp[i] = data[i-size];
		}
	} else {
		for (int i = bytes; i > this->size; i--) {
			temp[i] = data[i-bytes];
		}
	}
	
	// Создаем массив и копируем все элементы из временного массива
	this->size += bytes;
	this->buffer = new uint8_t[this->size];
	
	for (int i = 0; i < this->size; i++)
			this->buffer[i] = temp[i];
}

void BinaryStream::putByte(const uint8_t value) {
	uint8_t temp[this->size+1];
	temp[this->size] = value;
	
	if (this->buffer != nullptr) {
		for (int i = 0; i < this->size; i++)
				temp[i] = this->buffer[i];
		
		delete this->buffer;
		this->buffer = nullptr;
	}
	
	this->size++;
	this->buffer = new uint8_t[this->size];
	
	for (int i = 0; i < this->size; i++)
			this->buffer[i] = temp[i];
}

void BinaryStream::putLong(const int64_t value) {
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

void BinaryStream::putShort(const int16_t value) {
	this->putUShort(value);
}

void BinaryStream::putUShort(const uint16_t value) {
	if (endian == Endians::BENDIAN) {
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
	if (endian == Endians::BENDIAN) {
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