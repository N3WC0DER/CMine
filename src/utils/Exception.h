#pragma once
#include <string>
#include <sstream>

class Exception {
protected:
	std::stringstream error;
	
public:
	Exception() {}
	Exception(const Exception& ex) {
		this->error << ex.what();
	}
	
	Exception(const std::string str) {
		this->error << str;
	}
	
	std::string what() const {
		return this->error.str();
	}
	
	template<class T>
	Exception& operator<<(const T& value) {
		this->error << value;
		return *this;
	}
};