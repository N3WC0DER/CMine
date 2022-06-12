#include <iostream>
#include <ctime>
#include <thread>
#include <mutex>
#include <unistd.h>

#include "utils/Logger.h"
#include "network/Socket.h"
#include "network/PacketHandler.h"

#include "Server.h"

Server::Server() {
	unsigned int startTime = clock();
	this->getLogger()->info("[------------ CMine++ ------------]");
	this->getLogger()->info("Server started...");
	
	this->socket = std::make_unique<Socket>(this);
	this->socket->create();
	
	this->handler = std::make_unique<PacketHandler>(this->socket.get());
	
	unsigned int endTime = clock();
	this->getLogger()->info("Server started in " + std::to_string(endTime - startTime) + " ms");
	
	this->recvSocket = new std::thread([this] {
		this->socket->receive();
	});
	
	this->handlePacket = new std::thread([this] {
		this->handler->handlePacket();
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
	this->socket.reset();
	this->handler.reset();
	this->getLogger()->info("Server stopped.");
}

Logger* Server::getLogger() const {
	return this->logger;
}

void Server::shutdown() {
	this->getLogger()->info("Stopping the server...");
	
	this->socket->close();
	this->handler->close();
	
	this->handlePacket->join();
	this->recvSocket->join();
}