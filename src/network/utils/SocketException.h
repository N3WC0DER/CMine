#pragma once

#include "utils/Exception.h"

class SocketException : public Exception {
public:
	SocketException();
	SocketException(const SocketException& ex);
	SocketException(std::string str);
};