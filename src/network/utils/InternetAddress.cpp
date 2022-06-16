#include "InternetAddress.h"

InternetAddress::InternetAddress(std::string addr, uint16_t port, PortVersions version)
		: addr(addr), port(port), version(version) {}

std::string InternetAddress::getAddress() const {
	return this->addr;
}

uint16_t InternetAddress::getPort() const {
	return this->port;
}

PortVersions InternetAddress::getVersion() const {
	return this->version;
}

std::string InternetAddress::toString() const {
	std::stringstream ss;
	ss << this->getAddress() << ':' << this->getPort();
	return ss.str();
}

bool operator==(InternetAddress& addr1, InternetAddress& addr2) {
	return addr1.getAddress() == addr2.getAddress() && addr1.getPort() == addr2.getPort() && addr1.getVersion() == addr2.getVersion();
}

bool operator!=(InternetAddress& addr1, InternetAddress& addr2) {
	return !(addr1.getAddress() == addr2.getAddress() && addr1.getPort() == addr2.getPort() && addr1.getVersion() == addr2.getVersion());
}