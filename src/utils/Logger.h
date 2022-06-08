#pragma once
#include <iostream>

class Logger {
private:
	Logger();
	Logger(const Logger&);
	Logger operator=(Logger&);
	
	static tm* getCurrentTime();
	
	static std::unique_ptr<Logger> *instance = nullptr;
public:
	static std::unique_ptr<Logger> getInstance();
	static void info(std::string LOG_TAG, std::string data); //TODO: добавить вывод по выбранному потоку
	static void info(std::string data);
};