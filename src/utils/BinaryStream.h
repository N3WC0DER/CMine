#pragma once
#include "utils/Exception.h"

enum class Endians {
	BENDIAN,
	LENDIAN
};

class BinaryStream {
protected:
	uint8_t* buffer = nullptr;
	int size;
	int position;
	Endians endian = Endians::BENDIAN;
	
public:
	BinaryStream();
	BinaryStream(const uint8_t* buffer, const int size, Endians endian = Endians::BENDIAN);
	BinaryStream(const BinaryStream& stream);
	~BinaryStream();
	
	uint8_t* getBuffer() const;
	int getSize() const;
	
	void clear();
	
	bool feof() const;
	
	/**
	 * Вывод данных фиксированной длины из потока
	 */
	uint8_t* read(const size_t bytes);
	
	/** 
	 * Вывод байта из потока.
	 * @throws Exception
	 */
	uint8_t readByte();
	
	/**
	 * Вывод 8 подписанных байтов (signed long) из потока.
	 */
	int64_t readLong();
	
	/**
	 * Вывод 2 подписанных байтов (signed short) из потока.
	 */
	int16_t readShort();
	
	/**
	 * Вывод 2 неподписанных байтов (unsigned short) из потока.
	 */
	uint16_t readUShort();
	
	/**
	 * Вывод 4 подписанных байтов (int) из потока.
	 */
	int32_t readInt();
	
	/**
	 * Вывод 4 неподписанных байтов (unsigned int) из потока.
	 */
	uint32_t readUInt();
	
	/** 
	 * Вывод 1 бита (bool) из потока.
	 */
	bool readBoolean();
	
	/**
	 * Ввод данных фиксированной длины в поток.
	 */
	void put(const uint8_t* data, const size_t bytes);
	
	/** 
	 * Ввод байта в поток.
	 */
	void putByte(const uint8_t value);
	
	/**
	 * Ввод 8 подписанных байтов (signed long) в поток.
	 */
	void putLong(const int64_t value);
	
	/**
	 * Ввод 2 подписанных байтов (signed short) в поток.
	 */
	void putShort(const int16_t value);
	
	/**
	 * Ввод 2 неподписанных байтов (unsigned short) в поток.
	 */
	void putUShort(const uint16_t value);
	
	/**
	 * Ввод 2 подписанных байтов (int) в поток.
	 */
	void putInt(const int32_t value);
	
	/**
	 * Ввод 2 неподписанных байтов (unsigned int) в поток.
	 */
	void putUInt(const uint32_t value);
	
	/** 
	 * Ввод 1 бита (bool) в поток.
	 */
	void putBoolean(const bool value);
};