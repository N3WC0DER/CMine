#pragma once
#include <iostream>
#include <string>
#include <ctime>
#include <fstream>

#include "ServerInfo.h"
#include "utils/LogMessage.h"
#include "utils/Colors.h"

class Logger {
private:
	Logger();
	Logger(const Logger&) = delete;
	Logger operator=(Logger&) = delete;
	
	static std::unique_ptr<Logger> instance;
	
	inline tm* getCurrentTime();
	
	const std::string fileName = "../server.log";
public:
	~Logger();
	
	static Logger* getInstance();

	void info(const std::string data);
	void info(const LogMessage& msg);
	
	void notice(const std::string data);
	void notice(const LogMessage& msg);
	
	void warning(const std::string data);
	void warning(const LogMessage& msg);
	
	void debug(const std::string data);
	void debug(const LogMessage& msg);
	
	void error(const std::string data);
	void error(const LogMessage& msg);
	
	void critical(const std::string data);
	void critical(const LogMessage& msg);
};
