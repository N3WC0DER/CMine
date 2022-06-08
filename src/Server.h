#pragma once
#include "Logger.h"

class Server {
private:
	std::unique_ptr<Logger> logger;
public:
	Server();
};