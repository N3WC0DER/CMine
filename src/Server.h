#pragma once
#include <iostream>
#include <ctime>
#include <thread>
#include <unistd.h>
#include <sstream>
#include <map>

#include "utils/Logger.h"
#include "utils/Exception.h"
#include "network/Socket.h"
#include "ServerInfo.h"

enum class State {
	RUNNING,
	STARTED,
	STOPPING,
	STOPPED
};

class Socket; // циклическая зависимость

class Server {
private:
	std::unique_ptr<Socket> socket = nullptr;
	
	std::unique_ptr<std::thread> recvSocket = nullptr;
	std::unique_ptr<std::thread> handleSessions = nullptr;
	
	uint64_t serverGUID;
	
	State state = State::STOPPED;
	
public:
	Server();
	~Server();
	
	void start();
	
	Logger* getLogger() const;
	uint64_t getGUID() const;
	
	/** Состояние сервера */
	State getState() const;
	bool isShutdown() const;
	
	
	/** Выключение сервера */
	void shutdown();
};