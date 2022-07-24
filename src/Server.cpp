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
	srand(time(0)); // Для GUID
	unsigned int startTime = clock();
	Logger::getInstance()->info(LogMessage() << "[----------- " << ServerInfo::NAME << " " << ServerInfo::VERSION << " -----------]");
	Logger::getInstance()->info("Server is running...");
	Mutex::serverState.lock();
	this->state = State::RUNNING;
	Mutex::serverState.unlock();
	
	// Генерация GUID
	this->serverGUID = rand();
	this->serverGUID <<= 15;
	this->serverGUID |= rand();
	this->serverGUID <<= 15;
	this->serverGUID |= rand();
	this->serverGUID <<= 15;
	this->serverGUID |= rand();
	this->serverGUID <<= 3;
	this->serverGUID |= rand() & 0b111;
	
	// Сокет
	this->socket = std::make_unique<Socket>(this);
	this->socket->create();
	
	unsigned int endTime = clock();
	Logger::getInstance()->info(LogMessage() << "Server started in " << (double) (endTime - startTime) / (double) CLOCKS_PER_SEC * 1000.0 << " ms");
	
	// Прослушивание сокета
	this->recvSocket = std::make_unique<std::thread>([this] {
			this->socket->receive();
	});
	
	// Обновление сессий
	this->handleSessions = std::make_unique<std::thread>([this] {
		this->socket->getSessionManager()->updateSessions();
	});
	
	Mutex::serverState.lock();
	this->state = State::STARTED;
	Mutex::serverState.unlock();
	
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

State Server::getState() const {
	Mutex::serverState.lock();
	return this->state;
	Mutex::serverState.unlock();
}

bool Server::isShutdown() const {
	return this->state == State::STOPPED || this->state == State::STOPPING;
}

void Server::shutdown() {
	Logger::getInstance()->info("Stopping the server...");
	Mutex::serverState.lock();
	this->state = State::STOPPING;
	Mutex::serverState.unlock();
	
	if (socket != nullptr) {
		this->recvSocket->join();
		this->recvSocket.reset();
		if (this->socket->getSessionManager() != nullptr) {
			this->handleSessions->join();
			this->handleSessions.reset();
		}
		
		this->socket.reset();
	}
	
	Mutex::serverState.lock();
	this->state = State::STOPPED;
	Mutex::serverState.unlock();
}
