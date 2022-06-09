#include <iostream>
#include <ctime>
#include <fstream>
#include "utils/Logger.h"

std::unique_ptr<Logger> Logger::instance = nullptr;

Logger::Logger() {}
Logger::~Logger() {
	std::ofstream log(fileName, std::ios_base::app);
	log << "\n\n\n";
	log.close();
}
/** @return Logger* */
Logger* Logger::getInstance() {
	if (instance.get() == nullptr)
			instance.reset(new Logger);
	return instance.get();
}

tm* Logger::getCurrentTime() {
	time_t now = time(0);
	tm *localTime = localtime(&now);
	return localTime;
}

/**
 * выводит в консоль информацию, переданную из параметра
 * @param string data
 */
void Logger::info(std::string data) {
	tm *currentTime = Logger::getCurrentTime();
	std::ofstream log(fileName, std::ios_base::app);
	std::cout << "{" << currentTime->tm_hour << ":" << currentTime->tm_min << ":" << currentTime->tm_sec << "}"
			<< " " << data << std::endl;
	log << "{" << currentTime->tm_hour << ":" << currentTime->tm_min << ":" << currentTime->tm_sec << "}"
			<< " " << data << std::endl;
	log.close();
}