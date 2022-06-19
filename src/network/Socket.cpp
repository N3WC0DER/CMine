#include "Socket.h"

Socket::Socket(Server* server, uint16_t port)
		: server(server), port(port) {}

Socket::~Socket() {}

Server* Socket::getServer() const {
	return this->server;
}

Logger* Socket::getLogger() const {
	return this->logger;
}

Packet* Socket::getPacket() {
	if (this->packetQueue.empty())
			return nullptr;
	Packet* packet = this->packetQueue.front();
	this->packetQueue.pop();
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

void Socket::receive() {
	while (true) {
		Mutex::recvSocket.lock();
		if (this->stopped){
			Mutex::recvSocket.unlock();
			break;
		}
		Mutex::recvSocket.unlock();
		
		uint8_t packetData[1024 * 8];
		
#if PLATFORM == PLATFORM_WINDOWS
		typedef int socklen_t;
#endif

		sockaddr_in from;
		socklen_t fromLength = sizeof(from);

		int recvBytes = ::recvfrom(this->socket, (char*)packetData, sizeof(packetData), 0, (sockaddr*)&from, &fromLength);
		
		if (recvBytes >= 0) {
			// Отправитель
			auto addr = std::make_shared<InternetAddress>(inet_ntoa(from.sin_addr), ntohs(from.sin_port));
			
			uint8_t pid = packetData[0];
			if (pid == ID::UNCONNECTED_PING) {
				this->getLogger()->info("Handle UnconnectedPing from {" + addr->toString() + "}");
				
				auto buffer = std::make_shared<PacketSerializer>(packetData, recvBytes);
				
				// Вытаскиваем данные из пакета
				auto ping = std::make_shared<UnconnectedPing>();
				ping->decode(buffer.get());
				buffer->clear();
				
				// Пакет для отправления
				auto packet = std::make_shared<UnconnectedPong>();
				packet->pingTime = ping->pingTime; // pingTime из UnconnectedPing
				packet->serverGUID = this->getServer()->getGUID(); // GUID сервера
				
				std::stringstream serverID;
				serverID << "MCPE;" << "CMine++;" << "390;" << "1.1.5;" << "0;" << "100;" << this->getServer()->getGUID() << ";"
								 << "CMine++;" << "Survival;" << "1;" << "19132;" << "19133;";
				packet->serverID = serverID.str();
				serverID.clear();
				
				// Сериализуем пакет 
				packet->encode(buffer.get());
				
				// Отправляем
				this->send(buffer->getBuffer(), buffer->getSize(), from);
			} else {
				std::stringstream buff;
				buff << "Handle packet: " << (int) packetData[0] << " from {" << addr->toString() << "}";
				this->getLogger()->info(buff.str());
			}
		}
	}
}

void Socket::send(uint8_t* buffer, int packetSize, sockaddr_in& from) {
	int sendBytes = ::sendto(this->socket, (char*) buffer, packetSize, 0, (sockaddr*)&from, sizeof(from));
	
	if (sendBytes != packetSize) {
		this->getLogger()->info("Send error");
	}
}

// IP — inet_ntoa(from.sin_addr)
// Port — ntohs(from.sin_port)

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
