#pragma once
#include "Socket.h"

class Socket; // циклическая зависимость

class PacketHandler {
private:
	/** @var Socket* */
	Socket* socket;
	
	bool stopped = false;
	
public:
	PacketHandler(Socket* socket);
	
	/**
	 * Обработка пакетов
	 */
	void handlePacket();
	
	void close();
};