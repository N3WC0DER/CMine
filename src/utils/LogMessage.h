#pragma once
#include <sstream>

class LogMessage {
private:
	std::stringstream buffer;
	
public:
	LogMessage();
	LogMessage(const LogMessage& msg);
	LogMessage(const std::string str);
	
	std::string getMessage() const;
	
	template<class T>
	LogMessage& operator<<(const T& value) {
		this->buffer << value;
		return *this;
	}
};