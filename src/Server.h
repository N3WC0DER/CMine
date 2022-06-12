#pragma once
#include <mutex>

#include "utils/Logger.h"
#include "network/Socket.h"
#include "network/PacketHandler.h"

class Socket; // циклическая зависимость

class Server {
private:
	Server();
	Server(const Server&) = delete;
	Server operator=(Server&) = delete;
	
	/** @var std::unique_ptr<Server> */
	static std::unique_ptr<Server> instance;
	
	/** @var Logger* */
	Logger* logger = Logger::getInstance();
	/** @var std::unique_ptr<Socket> */
	std::unique_ptr<Socket> socket = nullptr;
	/** @var std::unique_ptr<PacketHandler> */
	std::unique_ptr<PacketHandler> handler = nullptr;
	
	/** @var std::thread */
	std::thread *recvSocket;
	/** @var std::thread */
	std::thread *handlePacket;
	
public:
	~Server();
	
	static void init();
	
	/** return Server* */
	static Server* getInstance();
	
	/** @return Logger* */
	Logger* getLogger() const;
	
	/** Выключение сервера */
	void shutdown();
};