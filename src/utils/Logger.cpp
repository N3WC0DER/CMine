#include "Logger.h"

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

inline tm* Logger::getCurrentTime() {
	time_t now = time(0);
	tm *localTime = localtime(&now);
	return localTime;
}

void Logger::info(const std::string data) {
	std::scoped_lock<std::mutex> lock(this->mutex);
	tm *currentTime = Logger::getCurrentTime();
	std::ofstream log(this->fileName, std::ios_base::app);
	std::cout << "{" << std::setw(2) << std::setfill('0') << currentTime->tm_mday << "—" << std::setw(2) << currentTime->tm_mon << "|" << std::setw(2) << currentTime->tm_hour << ":" << std::setw(2) << currentTime->tm_min << ":" << std::setw(2) << currentTime->tm_sec << "}"
						<< std::setfill(' ') << std::setw(9) << Colors::CYAN << " [" << Colors::CLEAR << "info" << Colors::CYAN << "] " << Colors::GREEN << "-> " << Colors::CLEAR << data << Colors::CLEAR << std::endl;
	log << "{" << std::setw(2) << std::setfill('0') << currentTime->tm_mday << "—" << std::setw(2) << currentTime->tm_mon << "|" << std::setw(2) << currentTime->tm_hour << ":" << std::setw(2) << currentTime->tm_min << ":" << std::setw(2) << currentTime->tm_sec << "}"
			<< std::setfill(' ') << std::setw(15) << " [info] -> " << data << std::endl;
	log.close();
}

void Logger::info(const LogMessage& msg) {
	this->info(msg.getMessage());
}

void Logger::notice(const std::string data) {
	std::scoped_lock<std::mutex> lock(this->mutex);
	tm *currentTime = Logger::getCurrentTime();
	std::ofstream log(this->fileName, std::ios_base::app);
	std::cout << "{" << std::setw(2) << std::setfill('0') << currentTime->tm_mday << "—" << std::setw(2) << currentTime->tm_mon << "|" << std::setw(2) << currentTime->tm_hour << ":" << std::setw(2) << currentTime->tm_min << ":" << std::setw(2) << currentTime->tm_sec << "}"
						<< std::setfill(' ') << std::setw(7) << Colors::CYAN << " [" << Colors::CLEAR << "notice" << Colors::CYAN << "] " << Colors::GREEN << "-> " << Colors::BLUE << data << Colors::CLEAR << std::endl;
	log << "{" << std::setw(2) << std::setfill('0') << currentTime->tm_mday << "—" << std::setw(2) << currentTime->tm_mon << "|" << std::setw(2) << currentTime->tm_hour << ":" << std::setw(2) << currentTime->tm_min << ":" << std::setw(2) << currentTime->tm_sec << "}"
			<< std::setfill(' ') << std::setw(15) << " [notice] -> " << data << std::endl;
	log.close();
}

void Logger::notice(const LogMessage& msg) {
	this->notice(msg.getMessage());
}

void Logger::warning(const std::string data) {
	std::scoped_lock<std::mutex> lock(this->mutex);
	tm *currentTime = Logger::getCurrentTime();
	std::ofstream log(this->fileName, std::ios_base::app);
	std::cout << "{" << std::setw(2) << std::setfill('0') << currentTime->tm_mday << "—" << std::setw(2) << currentTime->tm_mon << "|" << std::setw(2) << currentTime->tm_hour << ":" << std::setw(2) << currentTime->tm_min << ":" << std::setw(2) << currentTime->tm_sec << "}"
						<< std::setfill(' ') << std::setw(6) << Colors::CYAN << " [" << Colors::CLEAR << "warning" << Colors::CYAN << "] " << Colors::GREEN << "-> " << Colors::YELLOW << data << Colors::CLEAR << std::endl;
	log << "{" << std::setw(2) << std::setfill('0') << currentTime->tm_mday << "—" << std::setw(2) << currentTime->tm_mon << "|" << std::setw(2) << currentTime->tm_hour << ":" << std::setw(2) << currentTime->tm_min << ":" << std::setw(2) << currentTime->tm_sec << "}"
			<< std::setfill(' ') << std::setw(15) << " [warning] -> " << data << std::endl;
	log.close();
}

void Logger::warning(const LogMessage& msg) {
	this->warning(msg.getMessage());
}

void Logger::debug(const std::string data) {
	if (ServerInfo::DEVELOP) {
		std::scoped_lock<std::mutex> lock(this->mutex);
		tm *currentTime = Logger::getCurrentTime();
		std::ofstream log(this->fileName, std::ios_base::app);
		std::cout << "{" << std::setw(2) << std::setfill('0') << currentTime->tm_mday << "—" << std::setw(2) << currentTime->tm_mon << "|" << std::setw(2) << currentTime->tm_hour << ":" << std::setw(2) << currentTime->tm_min << ":" << std::setw(2) << currentTime->tm_sec << "}"
							<< std::setfill(' ') << std::setw(8) << Colors::CYAN << " [" << Colors::CLEAR << "debug" << Colors::CYAN << "] " << Colors::GREEN << "-> " << Colors::MAGENTA << data << Colors::CLEAR << std::endl;
		log << "{" << std::setw(2) << std::setfill('0') << currentTime->tm_mday << "—" << std::setw(2) << currentTime->tm_mon << "|" << std::setw(2) << currentTime->tm_hour << ":" << std::setw(2) << currentTime->tm_min << ":" << std::setw(2) << currentTime->tm_sec << "}"
				<< std::setfill(' ') << std::setw(15) << " [debug] -> " << data << std::endl;
		log.close();
	}
}

void Logger::debug(const LogMessage& msg) {
	this->debug(msg.getMessage());
}

void Logger::error(const std::string data) {
	std::scoped_lock<std::mutex> lock(this->mutex);
	tm *currentTime = Logger::getCurrentTime();
	std::ofstream log(this->fileName, std::ios_base::app);
	std::cout << "{" << std::setw(2) << std::setfill('0') << currentTime->tm_mday << "—" << std::setw(2) << currentTime->tm_mon << "|" << std::setw(2) << currentTime->tm_hour << ":" << std::setw(2) << currentTime->tm_min << ":" << std::setw(2) << currentTime->tm_sec << "}"
						<< std::setfill(' ') << std::setw(8) << Colors::CYAN << " [" << Colors::CLEAR << "error" << Colors::CYAN << "] " << Colors::GREEN << "-> " << Colors::RED << data << Colors::CLEAR << std::endl;
	log << "{" << std::setw(2) << std::setfill('0') << currentTime->tm_mday << "—" << std::setw(2) << currentTime->tm_mon << "|" << std::setw(2) << currentTime->tm_hour << ":" << std::setw(2) << currentTime->tm_min << ":" << std::setw(2) << currentTime->tm_sec << "}"
			<< std::setfill(' ') << std::setw(15) << " [error] -> " << data << std::endl;
	log.close();
}

void Logger::error(const LogMessage& msg) {
	this->error(msg.getMessage());
}

void Logger::critical(const std::string data) {
	std::scoped_lock<std::mutex> lock(this->mutex);
	tm *currentTime = Logger::getCurrentTime();
	std::ofstream log(this->fileName, std::ios_base::app);
	std::cout << "{" << std::setw(2) << std::setfill('0') << currentTime->tm_mday << "—" << std::setw(2) << currentTime->tm_mon << "|" << std::setw(2) << currentTime->tm_hour << ":" << std::setw(2) << currentTime->tm_min << ":" << std::setw(2) << currentTime->tm_sec << "}"
						<< std::setfill(' ') << Colors::CYAN << " [" << Colors::CLEAR << "CRITICAL" << Colors::CYAN << "] " << Colors::GREEN << "-> " << Colors::RED << Colors::BOLD << data << Colors::CLEAR << std::endl;
	log << "{" << std::setw(2) << std::setfill('0') << currentTime->tm_mday << "—" << std::setw(2) << currentTime->tm_mon << "|" << std::setw(2) << currentTime->tm_hour << ":" << std::setw(2) << currentTime->tm_min << ":" << std::setw(2) << currentTime->tm_sec << "}"
			<< std::setfill(' ') << std::setw(15) << " [CRITICAL] -> " << data << std::endl;
	log.close();
}

void Logger::critical(const LogMessage& msg) {
	this->critical(msg.getMessage());
}