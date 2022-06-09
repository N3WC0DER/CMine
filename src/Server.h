#pragma once
#include "utils/Logger.h"
//#include "network/Socket.h"

class Server {
private:
	Logger* logger = Logger::getInstance();
	//std::shared_ptr<Socket> socket = nullptr;
	
public:
	Server();
	~Server();
	
	Logger* getLogger() const;
};