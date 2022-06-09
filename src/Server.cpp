#include <iostream>
#include <ctime>

#include "utils/Logger.h"
#include "network/Socket.h"

#include "Server.h"

Server::Server() {
	unsigned int startTime = clock();
	this->getLogger()->info("[------------ CMine++ ------------]");
	this->getLogger()->info("Server started...");
	
	this->socket = std::make_unique<Socket>(this);
	this->socket->create();
	
	unsigned int endTime = clock();
	this->getLogger()->info("Server started in " + std::to_string(endTime - startTime) + " ms");
}

Server::~Server() {}

Logger* Server::getLogger() const {
	return this->logger;
}