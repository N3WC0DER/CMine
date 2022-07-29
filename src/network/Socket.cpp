#include "Socket.h"

#include <iostream>

Socket::Socket(Server* server, uint16_t port)
		: server(server), port(port) {
	this->startTime = time(0);
}

Socket::~Socket() {
	this->close();
}

Server* Socket::getServer() const {
	return this->server;
}

SessionManager* Socket::getSessionManager() const {
	return this->sessionManager.get();
}

void Socket::create() {
	this->socket = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (this->socket <= 0) {
		throw SocketException("Socket not created!\nPlease restart server.");
	}
	Logger::getInstance()->debug("Socket created!");
	
#if PLATFORM == PLATFORM_WINDOWS
	WSADATA WsaData;
	
	if (WSAStartup(MAKEWORD(2,2), &WsaData) != NO_ERROR) {
		throw SocketException("[Windows] Error socket create\nPlease restart server.");
	}
#endif

	sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons((unsigned short) this->port);
	
	if (::bind(this->socket, (const sockaddr*) &address, sizeof(sockaddr_in)) < 0) {
		throw SocketException("Error socket bind\nPlease restart server.");
	}
	Logger::getInstance()->debug("Socket binded!");
	
#if PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX
	int nonBlocking = 1;
	if (fcntl(this->socket, F_SETFL, O_NONBLOCK, nonBlocking) == -1) {
		throw SocketException("Failed to set non-blocking socket\nPlease restart server.");
	}

#elif PLATFORM == PLATFORM_WINDOWS
	DWORD nonBlocking = 1;
	if (ioctlsocket(this->socket, FIONBIO, &nonBlocking) != 0) {
		throw SocketException("Failed to set non-blocking socket\nPlease restart server.");
	}
#endif
	
	this->sessionManager = std::make_unique<SessionManager>(this);
}

void Socket::receive() {
	while (!this->getServer()->isShutdown()) {
		try {
			uint8_t packetData[1024 * 8];
			
	#if PLATFORM == PLATFORM_WINDOWS
			typedef int socklen_t;
	#endif
	
			sockaddr_in from;
			socklen_t fromLength = sizeof(from);
	
			int recvBytes = ::recvfrom(this->socket, (char*)packetData, sizeof(packetData), 0, (sockaddr*)&from, &fromLength);
			
			if (recvBytes >= 0) {
				auto buffer = std::make_unique<PacketSerializer>(packetData, recvBytes);
				
				// Sender
				auto addr = std::make_unique<InternetAddress>(inet_ntoa(from.sin_addr), ntohs(from.sin_port));
				
				uint8_t pid = packetData[0];
				if (pid == ID::UNCONNECTED_PING || pid == ID::UNCONNECTED_PING_OPEN_CONNECTIONS) {
					Logger::getInstance()->debug(LogMessage() << "Handle UnconnectedPing " << (int) pid << " from {" << addr->toString() << "}");
					
					// Getting data from a packet
					auto ping = std::make_unique<UnconnectedPing>();
					ping->decode(buffer.get());
					buffer->clear();
					
					// Preparing a packet for shipment
					auto packet = std::make_unique<UnconnectedPong>();
					packet->pingTime = ping->pingTime;
					packet->serverGUID = this->getServer()->getGUID();
					
					std::stringstream serverID;
					serverID << "MCPE;" << "CMine++ Server;" << "390;" << "1.1.5;" << "0;" << "100;" << this->getServer()->getGUID() << ";"
									 << "CMine++ Server;" << "Survival;" << "1;" << "19132;" << "19133;";
					packet->serverID = serverID.str();
					serverID.clear();
					
					// Serializing the packet
					packet->encode(buffer.get());
				} else if (pid == ID::OPEN_CONNECTION_REQUEST_1) {
					Logger::getInstance()->debug(LogMessage() << "Handle OpenConnectionRequest1 from {" << addr->toString() << "}");
					
					// Getting data from a packet
					auto request = std::make_unique<OpenConnectionRequest1>();
					request->decode(buffer.get());
					buffer->clear();
					
					// Checking the protocol version
					if (request->protocolVersion != ServerInfo::RAKLIB_PROTOCOL_VERSION) {
						// Preparing a packet for shipment
						auto packet = std::make_unique<IncompatibleProtocol>();
						packet->protocolVersion = ServerInfo::RAKLIB_PROTOCOL_VERSION;
						packet->serverGUID = this->getServer()->getGUID();
						
						// Serializing the packet
						packet->encode(buffer.get());
					} else {
						// Preparing a packet for shipment
						auto packet = std::make_unique<OpenConnectionReply1>();
						packet->serverGUID = this->getServer()->getGUID();
						packet->security = false;
						packet->MTU = request->MTU + 46;
						
						// Serializing the packet
						packet->encode(buffer.get());
					}
				} else if (pid == ID::OPEN_CONNECTION_REQUEST_2) {
					Logger::getInstance()->debug(LogMessage() << "Handle OpenConnectionRequest2 from {" << addr->toString() << "}");
					
					// Getting data from a packet
					auto request = std::make_unique<OpenConnectionRequest2>();
					request->decode(buffer.get());
					buffer->clear();
					
					// Preparing a packet for shipment
					auto packet = std::make_unique<OpenConnectionReply2>();
					packet->serverGUID = this->getServer()->getGUID();
					packet->clientAddr = addr.get();
					packet->MTU = request->MTU;
					packet->encryption = false;
					
					// Serializing the packet
					packet->encode(buffer.get());
					
					// Create a session
					this->sessionManager->createSession(addr.get(), request->MTU, request->userGUID);
				} else if (pid == ID::DISCONNECT) {
					buffer->clear();
					Session* session = this->sessionManager->getSession(addr.get());
					if (session == nullptr)
							throw SocketException() << "Non-created session {" << addr->toString() << "} sent a disconnect packet.";
					
					this->sessionManager->closeSession(session);
				} else { // Encapsulated
					Session* session = this->sessionManager->getSession(addr.get());
					if (session == nullptr)
							throw SocketException() << "Non-created session {" << addr->toString() << "} sent a packet: ID " << (int) packetData[0] << " size " << recvBytes;
					
					session->receivePacket(buffer.get());
					buffer->clear();
				}
				// Sending
				if (buffer->getSize() > 0)
						this->send(buffer->getBuffer(), buffer->getSize(), from);
			}
		} catch (const Exception& ex) {
			Logger::getInstance()->warning(LogMessage() << ex.what());
			continue;
		}
	}
}

void Socket::send(const uint8_t* buffer, const int packetSize, sockaddr_in& from) {
	int sendBytes = ::sendto(this->socket, (char*) buffer, packetSize, 0, (sockaddr*)&from, sizeof(from));
	
	if (sendBytes != packetSize) {
		Logger::getInstance()->warning(LogMessage() << "Packet " << (int) buffer[0] << " not sent");
	}
}

void Socket::send(const uint8_t* buffer, const int packetSize, const InternetAddress* addr) {
	sockaddr_in from;
	from.sin_family = AF_INET;
	from.sin_addr.s_addr = inet_addr(addr->getAddress().c_str());
	from.sin_port = htons((unsigned short) addr->getPort());
	
	this->send(buffer, packetSize, from);
}

void Socket::close() {
	this->sessionManager.reset();
	if (socket != -1) {
#if PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX
		//close(socket);
#elif PLATFORM == PLATFORM_WINDOWS
		closesocket(socket);
		WSACleanup();
#endif
	}
	Logger::getInstance()->debug("Socket closed");
}
