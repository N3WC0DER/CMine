#include "Server.h"

Server::Server() {
	try {
		this->start();
	} catch (const Exception& ex) {
		Logger::getInstance()->critical(LogMessage() << ex.what());
		this->shutdown();
	}
}

Server::~Server() {
	Logger::getInstance()->info("Server stopped.");
}

void Server::start() {
	srand(time(0)); // For GUID
	unsigned int startTime = clock();
	Logger::getInstance()->info(LogMessage() << "[--------- " << ServerInfo::NAME << " " << ServerInfo::VERSION << " ---------]");
	Logger::getInstance()->info("Server is running...");
	this->state = State::RUNNING;
	
	// GUID generation
	this->serverGUID = rand();
	this->serverGUID <<= 15;
	this->serverGUID |= rand();
	this->serverGUID <<= 15;
	this->serverGUID |= rand();
	this->serverGUID <<= 15;
	this->serverGUID |= rand();
	this->serverGUID <<= 3;
	this->serverGUID |= rand() & 0b111;
	
	// Thread manager
	ThreadManager::init(4);
	
	// Socket
	this->socket = std::make_unique<Socket>(this);
	this->socket->create();
	
	unsigned int endTime = clock();
	Logger::getInstance()->info(LogMessage() << "Server started in " << (double) (endTime - startTime) / (double) CLOCKS_PER_SEC * 1000.0 << " ms");
	
	// Socket listening
	this->recvSocket = ThreadManager::getInstance()->addTask(&Socket::receive, socket.get());
	
	this->state = State::STARTED;
	
	while (true) {
		std::string command;
		std::cin >> command;
	
		if (command == "stop"){
			this->shutdown();
			break;
		}
	}
}

uint64_t Server::getGUID() const {
	return this->serverGUID;
}

Server::State Server::getState() const {
	return this->state;
}

bool Server::isShutdown() const {
	return this->state == State::STOPPED || this->state == State::STOPPING;
}

void Server::shutdown() {
	Logger::getInstance()->info("Stopping the server...");
	this->state = State::STOPPING;
	
	this->recvSocket.get();
	if (socket != nullptr)
			this->socket.reset();
	
	this->state = State::STOPPED;
}
