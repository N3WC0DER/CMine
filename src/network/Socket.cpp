#include <iostream>

#define PLATFORM_WINDOWS  1
#define PLATFORM_MAC      2
#define PLATFORM_UNIX     3

#if defined(_WIN32)
#define PLATFORM PLATFORM_WINDOWS
#elif defined(__APPLE__)
#define PLATFORM PLATFORM_MAC
#else
#define PLATFORM PLATFORM_UNIX
#endif

#if PLATFORM == PLATFORM_WINDOWS
	#include <winsock2.h>
	#pragma comment(lib, "wsock32.lib")
#elif PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <fcntl.h>
#endif

#include "../Server.h"
#include "../utils/Mutex.h"

#include "Socket.h"

Socket::Socket(Server* server, uint16_t port)
		: server(server), port(port) {}

Socket::~Socket() {}

Server* Socket::getServer() const {
	return this->server;
}

Packet* Socket::getPacket() {
	if (this->packets.empty())
			return nullptr;
	Packet* packet = this->packets.front();
	this->packets.pop();
	return packet;
}

void Socket::create() {
	this->socket = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (this->socket <=0) {
		throw std::runtime_error("Socket not created!\nPlease restart server.");
	}
	this->getServer()->getLogger()->info("Socket created!");
	
#if PLATFORM == PLATFORM_WINDOWS
	WSADATA WsaData;
	
	if (WSAStartup(MAKEWORD(2,2), &WsaData) != NO_ERROR) {
		throw std::runtime_error("[Windows] Error socket create\nPlease restart server.");
	}
#endif
	
	sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons((unsigned short) this->port);
	
	if (::bind(this->socket, (const sockaddr*) &address, sizeof(sockaddr_in)) < 0) {
		throw std::runtime_error("Error socket bind\nPlease restart server.");
	}
	this->getServer()->getLogger()->info("Socket binded!");
	
#if PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX
	int nonBlocking = 1;
	if (fcntl(this->socket, F_SETFL, O_NONBLOCK, nonBlocking) == -1) {
		throw std::runtime_error("Failed to set non-blocking socket\nPlease restart server.");
	}

#elif PLATFORM == PLATFORM_WINDOWS
	DWORD nonBlocking = 1;
	if (ioctlsocket(this->socket, FIONBIO, &nonBlocking) != 0) {
		this->getServer()->getLogger()->info("Failed to set non-blocking socket\nPlease restart server.");
	}
#endif
}

void Socket::close() {
	Mutex::recvSocket.lock();
	this->stopped = true;
	Mutex::recvSocket.unlock();
	
#if PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX
	//close(socket);
#elif PLATFORM == PLATFORM_WINDOWS
	closesocket(socket);
	WSACleanup();
#endif
	this->getServer()->getLogger()->info("Socket closed");
}

void Socket::receive() {
	while (true) {
		Mutex::recvSocket.lock();
		if (this->stopped){
			Mutex::recvSocket.unlock();
			break;
		}
		Mutex::recvSocket.unlock();
		
		char packetData[this->mtu];
		
#if PLATFORM == PLATFORM_WINDOWS
		typedef int socklen_t;
#endif

		sockaddr_in from;
		socklen_t fromLength = sizeof(from);

		int recvBytes = ::recvfrom(this->socket, (char*)packetData, this->mtu, 0, (sockaddr*)&from, &fromLength);
		
		if (recvBytes >= 0) {
			Packet* packet = new Packet(recvBytes, (char*)packetData);
			this->packets.push(packet);
		}

		/*unsigned int from_address = ntohl(from.sin_addr.s_addr);
		unsigned int from_port = ntohs(from.sin_port);*/
	}
}