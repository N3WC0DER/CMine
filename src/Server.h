#pragma once
#include "utils/Logger.h"
#include "network/Socket.h"

class Socket; // циклическая зависимость

class Server {
private:
	Logger* logger = Logger::getInstance();
	std::unique_ptr<Socket> socket;
	
public:
	Server();
	~Server();
	
	Logger* getLogger() const;
};