#pragma once

enum Reliability {
	UNRELIABLE,
	UNRELIABLE_SEQUENCED,
	RELIABLE,
	RELIABLE_ORDERED,
	RELIABLE_SEQUENCED,
	UNRELIABLE_WITH_ACK_RECEIPT,
	RELIABLE_WITH_ACK_RECEIPT,
	RELIABLE_ORDERED_WITH_ACK_RECEIPT
};

namespace PacketReliability {
	static bool isReliable(uint8_t reliability) {
		return reliability == RELIABLE || 
				 	 reliability == RELIABLE_ORDERED || 
					 reliability == RELIABLE_SEQUENCED || 
					 reliability == RELIABLE_WITH_ACK_RECEIPT || 
					 reliability == RELIABLE_ORDERED_WITH_ACK_RECEIPT;
	}
	
	static bool isSequenced(uint8_t reliability) {
		return reliability == UNRELIABLE_SEQUENCED ||
					 reliability == RELIABLE_SEQUENCED;
	}
	
	static bool isOrdered(uint8_t reliability) {
		return reliability == RELIABLE_ORDERED ||
					 reliability == RELIABLE_ORDERED_WITH_ACK_RECEIPT;
	}
	
	static bool isSequencedOrOrdered(uint8_t reliability) {
		return isSequenced(reliability) || isOrdered(reliability);
	}
}