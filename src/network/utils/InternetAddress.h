#pragma once
#include <string>
#include <sstream>

enum IPVersions {
	IPv4 = 4,
	IPv6 = 6
};

class InternetAddress {
private:

	std::string addr;
	uint16_t port;

	IPVersions version;
public:
	InternetAddress();
	InternetAddress(const InternetAddress& addr);
	InternetAddress(const std::string addr, const uint16_t port, IPVersions version = IPVersions::IPv4);

	std::string getAddress() const;
	uint16_t getPort() const;
	IPVersions getVersion() const;
	/** 
	 * {IP}:{Port}
	 */
	std::string toString() const;
	
	InternetAddress& operator=(const InternetAddress& addr);
	friend bool operator==(const InternetAddress& addr1, const InternetAddress& addr2);
	friend bool operator!=(const InternetAddress& addr1, const InternetAddress& addr2);
};