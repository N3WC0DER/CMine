#pragma once
#include <iostream>
#include <sstream>
#include <queue>

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
	#include <arpa/inet.h>
#endif

#include "Server.h"
#include "utils/Mutex.h"
#include "network/protocol/Packets.h"
#include "network/utils/InternetAddress.h"
#include "network/utils/SocketException.h"
#include "network/SessionManager.h"

class SessionManager; // cyclic dependency
class Server; // cyclic dependency

class Socket {
private:
	Server* server = nullptr;
	std::unique_ptr<SessionManager> sessionManager = nullptr;
	
	uint16_t port;
	int socket = -1;
	
	uint64_t startTime;
	
public:
	Socket(Server* server, uint16_t port = 19132);
	~Socket();
	
	/** @return Server* */
	Server* getServer() const;
	/** @return SessionManager* */
	SessionManager* getSessionManager() const;
	
	inline uint64_t getTime() const {
		return this->startTime - time(0);
	}
	
	/**
	 * creating a socket for further listening
	 */
	void create();
	
	/**
	 * Receiving data from a socket
	 */
	void receive();
	
	/** 
	 * Sending data
	 */
	void send(const uint8_t* buffer, const int packetSize, sockaddr_in& from);
	void send(const uint8_t* buffer, const int packetSize, const InternetAddress* addr);
	
	/**
	 * Closing socket
	 */
	void close();
};