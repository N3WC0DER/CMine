#include "Server.h"

Server::Server() {
	srand(time(0)); // Для GUID
	unsigned int startTime = clock();
	this->getLogger()->info("[------------ CMine++ ------------]");
	this->getLogger()->info("Server started...");

	this->serverGUID = rand();
	this->serverGUID <<= 15;
	this->serverGUID |= rand();
	this->serverGUID <<= 15;
	this->serverGUID |= rand();
	this->serverGUID <<= 15;
	this->serverGUID |= rand();
	this->serverGUID <<= 3;
	this->serverGUID |= rand() & 0b111;
	
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

uint64_t Server::getGUID() const {
	return this->serverGUID;
}

void Server::shutdown() {
	this->getLogger()->info("Stopping the server...");
	
	this->socket->close();
	
	this->recvSocket->join();
}