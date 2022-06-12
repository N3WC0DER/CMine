#pragma once
#include <mutex>

#include "utils/Logger.h"
#include "network/Socket.h"
#include "network/PacketHandler.h"

class Socket; // циклическая зависимость

class Server {
private:
	/** @var Logger* */
	Logger* logger = Logger::getInstance();
	/** @var std::unique_ptr<Socket> */
	std::unique_ptr<Socket> socket;
	/** @var std::unique_ptr<PacketHandler> */
	std::unique_ptr<PacketHandler> handler;
	
	/** @var std::thread */
	std::thread *recvSocket;
	/** @var std::thread */
	std::thread *handlePacket;
	
public:
	Server();
	~Server();
	
	/** @return Logger* */
	Logger* getLogger() const;
	
	/** Выключение сервера */
	void shutdown();
};