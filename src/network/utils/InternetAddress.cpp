#include "InternetAddress.h"

InternetAddress::InternetAddress() {}

InternetAddress::InternetAddress(const InternetAddress& addr) {
	this->addr = addr.getAddress();
	this->port = addr.getPort();
	this->version = addr.getVersion();
}

InternetAddress::InternetAddress(std::string addr, uint16_t port, IPVersions version)
		: addr(addr), port(port), version(version) {}

std::string InternetAddress::getAddress() const {
	return this->addr;
}

uint16_t InternetAddress::getPort() const {
	return this->port;
}

IPVersions InternetAddress::getVersion() const {
	return this->version;
}

std::string InternetAddress::toString() const {
	std::stringstream ss;
	ss << this->getAddress() << ':' << this->getPort();
	return ss.str();
}

InternetAddress& InternetAddress::operator=(const InternetAddress& addr) {
	this->addr = addr.getAddress();
	this->port = addr.getPort();
	this->version = addr.getVersion();
	return *this;
}

bool operator==(const InternetAddress& addr1, const InternetAddress& addr2) {
	return (addr1.getAddress() == addr2.getAddress()) && (addr1.getPort() == addr2.getPort()) && (addr1.getVersion() == addr2.getVersion());
}

bool operator!=(const InternetAddress& addr1, const InternetAddress& addr2) {
	return !(addr1.getAddress() == addr2.getAddress() && addr1.getPort() == addr2.getPort() && addr1.getVersion() == addr2.getVersion());
}