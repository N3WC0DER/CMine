#pragma once

#include "thread/ThreadManager.h"
#include "network/protocol/PacketReliability.h"
#include "network/protocol/PacketSerializer.h"
#include "network/protocol/Packets.h"
#include "network/utils/SocketException.h"
#include "network/utils/InternetAddress.h"
#include "network/ReliabilityLayer.h"
#include "network/Socket.h"
#include "utils/Logger.h"

class Socket; // cyclic dependency
class ReliabilityLayer; // cyclic dependency

class Session {
public:
	enum class State {
		CONNECTING,
		CONNECTED,
		DISCONNECTED
	};

private:
	Socket* socket = nullptr;
	std::unique_ptr<ReliabilityLayer> layer = nullptr;
	
	mutable std::mutex mutex;
	
	std::unique_ptr<InternetAddress> addr = nullptr;
	uint16_t MTU;
	int64_t GUID;
	
	std::atomic<State> state = State::CONNECTING;
	
	uint64_t lastUpdate = 0;
	
public:
	Session(Socket* sock, const InternetAddress* addr, const uint16_t MTU, const int64_t GUID);
	
	InternetAddress* getAddress() const;
	uint16_t getMTU() const;
	int64_t getGUID() const;
	Session::State getState() const;
	
	bool isConnected() const;
	
	void update();
	
	void disconnect(std::string reason);
	
	void receivePacket(PacketSerializer* buffer);
	void handleRawPacket(Datagram* datagram);
	void sendPacket(const PacketSerializer* packet);
};