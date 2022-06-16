#pragma once
#include <string>

enum class Endians {
		BENDIAN,
		LENDIAN
	};

class BinaryStream {
protected:
	char *buffer;
	int size;
	int position;
	Endians endian;
	
public:
	BinaryStream();
	BinaryStream(char* str, int size, Endians endian = Endians::LENDIAN);
	
	uint8_t readByte();
	int64_t readLong();
	short readShort();
	unsigned short readUShort();
	bool readBoolean();
	
	void putByte(uint8_t c);
	void putLong(int64_t l);
	void putShort(short s);
	void putUShort(unsigned short us);
	void putBoolean(bool b);
};