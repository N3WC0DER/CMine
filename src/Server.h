#pragma once
#include <iostream>
#include <ctime>
#include <thread>
#include <mutex>
#include <unistd.h>

#include "utils/Logger.h"
#include "network/Socket.h"

class Socket; // циклическая зависимость

class Server {
private:
	/** @var Logger* */
	Logger* logger = Logger::getInstance();
	/** @var std::unique_ptr<Socket> */
	std::unique_ptr<Socket> socket = nullptr;
	
	/** @var std::thread */
	std::thread *recvSocket;
	
public:
	Server();
	~Server();
	
	/** @return Logger* */
	Logger* getLogger() const;
	
	/** Выключение сервера */
	void shutdown();
};