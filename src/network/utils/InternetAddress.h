#pragma once
#include <string>
#include <sstream>

enum PortVersions {
	IPv4 = 4,
	IPv6 = 6
};

class InternetAddress {
private:
	std::string addr;
	uint16_t port;
	PortVersions version;
public:
	InternetAddress(std::string addr, uint16_t port, PortVersions version = PortVersions::IPv4);

	std::string getAddress() const;
	uint16_t getPort() const;
	PortVersions getVersion() const;
	std::string toString() const;
	
	friend bool operator==(InternetAddress& addr1, InternetAddress& addr2);
	friend bool operator!=(InternetAddress& addr1, InternetAddress& addr2);
};