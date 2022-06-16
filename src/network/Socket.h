#pragma once
#include <iostream>
#include <queue>

#define PLATFORM_WINDOWS  1
#define PLATFORM_MAC      2
#define PLATFORM_UNIX     3

#if defined(_WIN32)
#define PLATFORM PLATFORM_WINDOWS
#elif defined(__APPLE__)
#define PLATFORM PLATFORM_MAC
#else
#define PLATFORM PLATFORM_UNIX
#endif

#if PLATFORM == PLATFORM_WINDOWS
	#include <winsock2.h>
	#pragma comment(lib, "wsock32.lib")
#elif PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <fcntl.h>
	#include <arpa/inet.h>
#endif

#include "../Server.h"
#include "../utils/Mutex.h"
#include "../utils/BinaryStream.h"
#include "protocol/PacketSerializer.h"
#include "protocol/Packet.h"

class Server; // циклическая зависимость

class Socket {
private:
	/** @var Server* */
	Server* server;
	
	/** @var Logger* */
	Logger* logger = Logger::getInstance();
	
	/** @var uint16_t */
	uint16_t port;
	int socket;
	
	bool stopped = false;
	
	/** 
	 * Очередь пакетов
	 * @var std::queue<Packet*>
	 */
	std::queue<Packet*> packetQueue;
	
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