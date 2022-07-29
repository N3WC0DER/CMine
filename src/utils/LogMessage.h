#pragma once
#include <sstream>

class LogMessage {
private:
	std::stringstream buffer;
	
public:
	LogMessage() {}
	LogMessage(const LogMessage& msg) {
		this->buffer << msg.getMessage();
	}
	
	LogMessage(const std::string str) {
		this->buffer << str;
	}
	
	std::string getMessage() const {
		return this->buffer.str();
	}
	
	template<class T>
	LogMessage& operator<<(const T& value) {
		this->buffer << value;
		return *this;
	}
};