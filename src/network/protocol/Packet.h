#pragma once

class Packet {
private:
	//char header[64];
	char* payload;
	
	int size;
	
public:
	Packet(int recvBytes, char* payload);
	
	int getSize() const;
	
	/** 
	 * Декодирует полезную нагрузку пакета
	 * @return char*
	 */
	char* decodePayload();
};