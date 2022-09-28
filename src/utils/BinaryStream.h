#pragma once
#include "utils/Exception.h"

enum class Endian {
	BENDIAN,
	LENDIAN
};

class BinaryStream {
protected:
	uint8_t* buffer = nullptr;
	size_t size = 0;
	size_t capacity = 0;
	static constexpr uint8_t BLOCK = 4;
	mutable size_t position = 0;
	Endian endian = Endian::BENDIAN;
	
public:
	BinaryStream();
	BinaryStream(const uint8_t* buffer, const int size, const Endian endian = Endian::BENDIAN);
	BinaryStream(const BinaryStream& stream);
	~BinaryStream();
	
	uint8_t* getBuffer() const;
	int getSize() const;
	Endian getEndian() const;
	int getCapacity() const;
	
	void changeEndian();
	
	void clear();
	
	bool feof() const;
	
	/**
	 * Вывод данных фиксированной длины из потока
	 */
	uint8_t* read(const size_t bytes) const;
	
	/** 
	 * Вывод байта из потока.
	 * @throws Exception
	 */
	uint8_t readByte() const;
	
	/**
	 * Вывод 8 подписанных байтов (signed long) из потока.
	 */
	int64_t readLong() const;
	
	/**
	 * Вывод 2 подписанных байтов (signed short) из потока.
	 */
	int16_t readShort() const;
	
	/**
	 * Вывод 2 неподписанных байтов (unsigned short) из потока.
	 */
	uint16_t readUShort() const;
	
	/**
	 * Вывод 4 подписанных байтов (int) из потока.
	 */
	int32_t readInt() const;
	
	/**
	 * Вывод 4 неподписанных байтов (unsigned int) из потока.
	 */
	uint32_t readUInt() const;
	
	/** 
	 * Вывод 1 бита (bool) из потока.
	 */
	bool readBoolean() const;
	
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