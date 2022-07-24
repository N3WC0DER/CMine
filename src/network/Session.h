#pragma once
#include <thread>

#include "network/protocol/PacketReliability.h"
#include "network/protocol/PacketSerializer.h"
#include "network/protocol/Packets.h"
#include "network/utils/SocketException.h"
#include "network/utils/InternetAddress.h"
#include "network/ReliabilityLayer.h"
#include "network/Socket.h"
#include "utils/Logger.h"

class Socket; // циклическая зависимость
class ReliabilityLayer; // циклическая зависимость

class Session {
private:
	Socket* socket = nullptr;
	std::unique_ptr<ReliabilityLayer> layer = nullptr;
	
	std::unique_ptr<InternetAddress> addr = nullptr;
	uint16_t MTU;
	int64_t GUID;
	
	uint64_t lastUpdate = 0;
	
public:
	Session(Socket* sock, InternetAddress* addr, uint16_t MTU, int64_t GUID);
	
	InternetAddress* getAddress() const;
	uint16_t getMTU() const;
	int64_t getGUID() const;
	
	void update();
	
	void disconnect();
	
	void receivePacket(PacketSerializer* buffer);
	void handleRawPacket(Datagram* datagram);
	void sendPacket(const PacketSerializer* packet);
};