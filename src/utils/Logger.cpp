#include <iostream>
#include <ctime>
#include "Logger.h"

Logger::Logger() {
	
}

std:unique_ptr<Logger> Logger::getInstance() {
	if (instance == nullptr) {
		instance = std::make_unique<Logger>();
	}
	return instance;
}

tm* Logger::getCurrentTime() {
	time_t now = time(0);
	tm *localTime = localtime(&now);
	return localTime;
}

/**
 * выводит в консоль информацию, переданную из параметра
 * @param string LOG_TAG
 * @param string data
 */
void Logger::info(std::string LOG_TAG, std::string data) {
	tm *currentTime = Logger::getCurrentTime();
	std::cout << "{" << currentTime->tm_hour << ":" << currentTime->tm_min << ":" << currentTime->tm_sec << "}"
			<< " " << "[" << LOG_TAG << "]: " << data << std::endl;
}

/**
 * выводит в консоль информацию, переданную из параметра
 * @param string data
 */
void Logger::info(std::string data) {
	tm *currentTime = Logger::getCurrentTime();
	std::cout << "{" << currentTime->tm_hour << ":" << currentTime->tm_min << ":" << currentTime->tm_sec << "}"
			<< " " << "[Logger]: " << data << std::endl;
}