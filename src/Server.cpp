#include "Server.h"

Server::Server() {
	unsigned int startTime = clock();
	this->getLogger()->info("[------------ CMine++ ------------]");
	this->getLogger()->info("Server started...");
	
	this->socket = std::make_unique<Socket>(this);
	this->socket->create();
	
	unsigned int endTime = clock();
	this->getLogger()->info("Server started in " + std::to_string(endTime - startTime) + " ms");
	
	this->recvSocket = new std::thread([this] {
		this->socket->receive();
	});
	
	while (true) {
		std::string command;
		std::cin >> command;
	
		if (command == "stop"){
			this->shutdown();
			break;
		}
	}
}

Server::~Server() {
	this->getLogger()->info("Server stopped.");
}

Logger* Server::getLogger() const {
	return this->logger;
}

void Server::shutdown() {
	this->getLogger()->info("Stopping the server...");
	
	this->socket->close();
	
	this->recvSocket->join();
}