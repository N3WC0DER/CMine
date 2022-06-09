#include <iostream>
#include <ctime>

#include "utils/Logger.h"

#include "Server.h"

Server::Server() {
	unsigned int startTime = clock();
	this->getLogger()->info("[------------ CMine++ ------------]");
	this->getLogger()->info("Server started...");
	
	unsigned int endTime = clock();
	this->getLogger()->info("Server started in " + std::to_string(endTime - startTime) + " ms");
}

Server::~Server() {}

Logger* Server::getLogger() const {
	return this->logger;
}