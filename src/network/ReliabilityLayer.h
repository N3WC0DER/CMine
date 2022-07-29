#pragma once
#include <vector>
#include <map>
#include <unordered_map>
#include <functional>

#include "network/Session.h"
#include "network/protocol/PacketSerializer.h"
#include "network/protocol/Acknowledge.h"
#include "network/protocol/Datagram.h"
#include "network/protocol/EncapsulatedPacket.h"
#include "network/protocol/PacketReliability.h"

#include "utils/Logger.h"

class Session;

enum class QueuePriority {
	IMMEDIATE,
	UPDATE,
	FULLQ
};

class ReliabilityLayer {
private:
	static const uint8_t MAX_ORDER_CHANNELS = 32;
	static const uint8_t MAX_SPLIT_PART_COUNT = 128;
	static const uint8_t MAX_SPLIT_PACKETS = 4;
	
	uint16_t sessionMTU;
	
	std::function<void(const PacketSerializer*)> sendPacket;
	
	Acknowledge<ID::ACK> ACKQueue;
	Acknowledge<ID::NACK> NACKQueue;
	
	uint32_t lastRecvSequenceNumber;
	uint32_t recvSequenceNumber;
	
	std::vector<uint32_t> recvReliableIndex;
	
	std::vector<std::vector<std::unique_ptr<EncapsulatedPacket>>> splitPackets{MAX_SPLIT_PACKETS};
	
	std::vector<uint32_t> recvSequencedIndex{MAX_ORDER_CHANNELS};
	std::vector<uint32_t> recvOrderedIndex{MAX_ORDER_CHANNELS};
	std::vector<std::map<uint32_t, std::unique_ptr<EncapsulatedPacket>>> recvOrderedPackets{MAX_ORDER_CHANNELS};
	
	uint32_t sendSequenceNumber = 0;
	
	uint32_t sendReliableIndex = 0;
	
	uint16_t splitID = 0;
	
	std::vector<uint32_t> sendSequencedIndex{MAX_ORDER_CHANNELS};
	std::vector<uint32_t> sendOrderedIndex{MAX_ORDER_CHANNELS};
	
	std::vector<std::unique_ptr<EncapsulatedPacket>> normalQueue;
	std::vector<std::unique_ptr<EncapsulatedPacket>> updateQueue;
	std::unordered_map<uint32_t, std::unique_ptr<PacketSerializer>> recoveryQueue;
	
public:
	ReliabilityLayer(std::function<void(const PacketSerializer*)> sendPacket);
	
	size_t handleDatagram(Datagram* datagram);
	void handleEncapsulated(Datagram* datagram);
	bool handleSplit(EncapsulatedPacket* buffer);
	
	void sendEncapsulated(PacketSerializer* buffer, Reliability reliability, uint8_t orderChannel, QueuePriority priority = QueuePriority::UPDATE);
	void sendDatagram(std::vector<std::unique_ptr<EncapsulatedPacket>>& packets);
	void sendDatagram(EncapsulatedPacket* packet);
	
	friend class Session;
};