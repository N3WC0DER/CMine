#include "utils/Logger.h"

std::unique_ptr<Logger> Logger::instance = nullptr;

Logger::Logger() {
	// Очистка файла, если слишком большой
	int count = 0;
	std::ifstream log(this->fileName);
	while (log.ignore(std::numeric_limits<std::streamsize>::max(), '\n')) {
		if (!log.eof())
				count++;
	}
	if (count >= 10000) {
		std::ofstream temp(this->fileName, std::ios::out);
		temp.close();
	}
	log.close();
}
Logger::~Logger() {
	std::ofstream log(fileName, std::ios_base::app);
	log << "\n\n\n";
	log.close();
}

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

void Logger::info(std::string data) {
	tm *currentTime = Logger::getCurrentTime();
	std::ofstream log(this->fileName, std::ios_base::app);
	std::cout << "{" << currentTime->tm_hour << ":" << currentTime->tm_min << ":" << currentTime->tm_sec << "}"
			<< " " << data << std::endl;
	log << "{" << currentTime->tm_hour << ":" << currentTime->tm_min << ":" << currentTime->tm_sec << "}"
			<< " " << data << std::endl;
	log.close();
}