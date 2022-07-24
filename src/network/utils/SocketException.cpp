#include "SocketException.h"

SocketException::SocketException()
		: Exception("[SocketException]: ") {}

SocketException::SocketException(const SocketException& ex)
		: Exception("[SocketException]: ") {
	this->error << ex.what();
}

SocketException::SocketException(std::string str)
		: Exception("[SocketException]: ") {
	this->error << str;
}