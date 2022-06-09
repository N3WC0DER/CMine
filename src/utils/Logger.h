#pragma once
#include <ctime>
#include <memory>
#include <string>

class Logger {
private:
	Logger();
	Logger(const Logger&) = delete;
	Logger operator=(Logger&) = delete;
	
	tm* getCurrentTime();
	
	static std::unique_ptr<Logger> instance;
	const std::string fileName = "../server.log";
public:
	~Logger();
	
	static Logger* getInstance();
	void info(std::string data);
};
