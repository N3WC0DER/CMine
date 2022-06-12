#pragma once
#include <queue>

#include "../Server.h"
#include "protocol/Packet.h"

class Server; // циклическая зависимость

class Socket {
private:
	/** @var Server* */
	Server* server;
	
	/** @var uint16_t */
	uint16_t port;
	int socket;
	
	/** Maximum Transmition Unit */
	uint8_t mtu = 255;
	
	bool stopped = false;
	
	/** 
	 * Очередь пакетов
	 * @var std::queue<Packet*>
	 */
	std::queue<Packet*> packets;
	
public:
	Socket(Server* server, uint16_t port = 19132);
	~Socket();
	
	/** @return Server* */
	Server* getServer() const;
	
	/** @return Packet* */
	Packet* getPacket();
	
	/**
	 * Создание сокета для дальнейшего прослушивания
	 */
	void create();
	
	/**
	 * Получение данных из сокета
	 */
	void receive();
	
	/**
	 * Закрытие сокета
	 */
	void close();
};