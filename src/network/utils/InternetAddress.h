#pragma once
#include <string>
#include <sstream>

enum IPVersions {
	IPv4 = 4,
	IPv6 = 6
};

class InternetAddress {
private:
	/**
	 * IP, разделенный точкамм.
	 * @var std::string 
	 */
	std::string addr;
	/**
	 * Порт.
	 * @var uint16_t
	 */
	uint16_t port;
	/**
	 * Версия IP (IPv4 или IPv6).
	 * @var IPVersions
	 */
	IPVersions version;
public:
	InternetAddress(std::string addr, uint16_t port, IPVersions version = IPVersions::IPv4);

	/** @return std::string */
	std::string getAddress() const;
	/** @return uint16_t */
	uint16_t getPort() const;
	/** @return IPVersions */
	IPVersions getVersion() const;
	/** 
	 * {IP}:{Port}
	 * @return std::string
	 */
	std::string toString() const;
	
	friend bool operator==(InternetAddress& addr1, InternetAddress& addr2);
	friend bool operator!=(InternetAddress& addr1, InternetAddress& addr2);
};