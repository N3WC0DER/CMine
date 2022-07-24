#pragma once
#include <string>

#include "network/utils/InternetAddress.h"
#include "utils/BinaryStream.h"

class PacketSerializer : public BinaryStream{
private:
	/** OFFLINE_MESSAGE_DATA_ID */
	uint8_t magic[16] = {0x00, 0xff, 0xff, 0x00,
										0xfe, 0xfe, 0xfe, 0xfe, 
										0xfd, 0xfd, 0xfd, 0xfd, 
										0x12, 0x34, 0x56, 0x78};
	
public:
	PacketSerializer();
	PacketSerializer(const uint8_t* buffer, const int recvBytes);
	PacketSerializer(const PacketSerializer& buffer);
	
	/** 
	 * Вывод строки из потока.
	 * @return std::string
	 */
	std::string readString();
	
	/**
	 * Вывод адреса из потока.
	 * 1 байт для версии (4 или 6), 4 байта IP, 2 байта порт.
	 * @return InternetAddress*
	 */
	InternetAddress* readAddress();
	
	/** 
	 * Вывод 3 little-endian байтов из потока.
	 * @return uint8_t*
	 */
	uint32_t readLTriad();
	
	/**
	 * "Вывод" OFFLINE_MESSAGE_DATA_ID из потока.
	 * На самом деле просто переставляем позицию в потоке.
	 */
	void readMagic();
	
	/**
	 * Ввод строки в поток.
	 * @param std::string str 
	 */
	void putString(const std::string str);
	
	/** 
	 * Ввод адреса в поток.
	 * 1 байт для версии (4 или 6), 4 байта IP, 2 байта порт.
	 * @param InternetAddress* addr 
	 */
	void putAddress(const InternetAddress* addr);
	
	/**
	 * Ввод 3 little-endian байтов в поток.
	 * @param uint8_t* triad массив байтов
	 */
	void putLTriad(const uint32_t triad);
	
	/** 
	 * Ввод OFFLINE_MESSAGE_DATA_ID в поток.
	 */
	void putMagic();
};