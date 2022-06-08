#include <iostream>
#include "utils/Logger.h"

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

using namespace std;

inline bool initSockets() { // For Windows
#if PLATFORM == PLATFORM_WINDOWS
	WSADATA WsaData;
	return WSAStartup(MAKEWORD(2,2), &WsaData) == NO_ERROR;
#else
	return true;
#endif
}

inline void shutdownSockets() {
#if PLATFORM == PLATFORM_WINDOWS
	WSACleanup();
#endif
}

int port = 19132;

int main() {
	int handle = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (handle <=0) {
		Logger::info("Error socket create");
		return -1;
	}
	Logger::info("Socket created!");
	
	if (!initSockets()) {
		Logger::info("[Windows] Error socket create");
		return -1;
	}
	
	sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons((unsigned short) port);
	
	if (::bind(handle, (const sockaddr*) &address, sizeof(sockaddr_in)) < 0) {
		Logger::info("Error socket bind");
		return -1;
	}
	Logger::info("Socket binded!");
	
#if PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX
	int nonBlocking = 1;
	if (fcntl(handle, F_SETFL, O_NONBLOCK, nonBlocking) == -1) {
		Logger::info("failed to set non-blocking socket");
		return false;
	}

#elif PLATFORM == PLATFORM_WINDOWS
	DWORD nonBlocking = 1;
	if (ioctlsocket(handle, FIONBIO, &nonBlocking) != 0) {
		Logger::info("failed to set non-blocking socket");
		return false;
	}
#endif
	Logger::info("Data received...");
	while (true) {
		unsigned char packet_data[256];
		unsigned int maximum_packet_size = sizeof(packet_data);

#if PLATFORM == PLATFORM_WINDOWS
		typedef int socklen_t;
#endif

		sockaddr_in from;
		socklen_t fromLength = sizeof(from);

		int received_bytes = ::recvfrom(handle, (char*)packet_data, maximum_packet_size, 0, (sockaddr*)&from, &fromLength);
		
		if (received_bytes >= 0) {
			printf("%i\n", received_bytes);
			for (int i = 0; i < sizeof(packet_data); i++) {
				cout << packet_data[i];
			}
			break;
		}

		unsigned int from_address = ntohl(from.sin_addr.s_addr);
		unsigned int from_port = ntohs(from.sin_port);
	}
	
#if PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX
	//close(socket);
#elif PLATFORM == PLATFORM_WINDOWS
	closesocket(socket);
#endif
	shutdownSockets();
	return 0;
}