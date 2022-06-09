#pragma once
#include "../Server.h"

class Server; // циклическая зависимость

class Socket {
private:
	/** @var Server* */
	Server* server;
	
	uint16_t port;
	int socket;
	
	/** Maximum Transmition Unit */
	uint8_t mtu;
	
public:
	Socket(Server* server, uint16_t port = 19132);
	~Socket();
	
	Server* getServer() const;
	
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