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
	
	/** @var uint64_t */
	uint64_t serverGUID;
	
public:
	Server();
	~Server();
	
	/** @return Logger* */
	Logger* getLogger() const;
	
	/** @return uint64_t GUID */
	uint64_t getGUID() const;
	
	/** Выключение сервера */
	void shutdown();
};