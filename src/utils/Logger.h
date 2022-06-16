#pragma once
#include <iostream>
#include <ctime>
#include <memory>
#include <string>
#include <ctime>
#include <fstream>

class Logger {
private:
	Logger();
	Logger(const Logger&) = delete;
	Logger operator=(Logger&) = delete;
	
	/** Получение текущего времени
	 * @return tm* структура, содержащая текущее время
	 */
	tm* getCurrentTime();
	
	/** @var unique_ptr<Logger> */
	static std::unique_ptr<Logger> instance;
	
	/** Имя файла, куда сохраняются логи сервера
	 * @var std::string
	 */
	const std::string fileName = "../server.log";
public:
	~Logger();
	
	/** @return Logger* */
	static Logger* getInstance();
	/**
	 * выводит в консоль информацию, переданную из параметра
	 * @param string data
	 */
	void info(std::string data);
};
