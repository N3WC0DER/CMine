#pragma once
#include <string>

enum class Endians {
		BENDIAN,
		LENDIAN
	};

class BinaryStream {
protected:
	uint8_t *buffer = nullptr;
	int size;
	int position;
	Endians endian;
	
public:
	BinaryStream();
	BinaryStream(uint8_t* buffer, int size, Endians endian = Endians::BENDIAN);
	
	/** @return uint8_t* */
	uint8_t* getBuffer() const;
	/** @return int */
	int getSize() const;
	
	void clear();
	
	/** 
	 * Вывод байта из потока.
	 * @return uint8_t
	 */
	uint8_t readByte();
	
	/**
	 * Вывод 8 подписанных байтов (signed long) из потока.
	 * @return int64_t
	 */
	int64_t readLong();
	
	/**
	 * Вывод 2 подписанных байтов (signed short) из потока.
	 * @return short
	 */
	short readShort();
	
	/**
	 * Вывод 2 неподписанных байтов (unsigned short) из потока.
	 * @return unsigned short
	 */
	unsigned short readUShort();
	
	/** 
	 * Вывод 1 бита (bool) из потока.
	 * @return bool
	 */
	bool readBoolean();
	
	/** 
	 * Ввод байта в поток.
	 * @param uint8_t c
	 */
	void putByte(uint8_t c);
	
	/**
	 * Ввод 8 подписанных байтов (signed long) в поток.
	 * @param int64_t l
	 */
	void putLong(int64_t l);
	
	/**
	 * Ввод 2 подписанных байтов (signed short) в поток.
	 * @param short s
	 */
	void putShort(short s);
	
	/**
	 * Ввод 2 неподписанных байтов (unsigned short) в поток.
	 * @paran unsigned short us
	 */
	void putUShort(unsigned short us);
	
	/** 
	 * Ввод 1 бита (bool) в поток.
	 * @param bool b
	 */
	void putBoolean(bool b);
};