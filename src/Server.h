#pragma once
#include <iostream>
#include <ctime>
#include <unistd.h>
#include <future>

#include "utils/Logger.h"
#include "utils/Exception.h"
#include "network/Socket.h"
#include "ServerInfo.h"

class Socket; // cyclic dependency

class Server {
public:
	enum class State {
		RUNNING,
		STARTED,
		STOPPING,
		STOPPED
	};

private:
	std::unique_ptr<Socket> socket = nullptr;
	
	std::future<void> recvSocket;
	std::future<void> handleSessions;
	
	uint64_t serverGUID;
	
	std::atomic<State> state = State::STOPPED;
	
public:
	Server();
	~Server();
	
	void start();
	
	Logger* getLogger() const;
	uint64_t getGUID() const;
	
	/** Состояние сервера */
	Server::State getState() const;
	bool isShutdown() const;
	
	
	/** Выключение сервера */
	void shutdown();
};