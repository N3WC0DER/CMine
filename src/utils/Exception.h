#pragma once
#include <string>
#include <sstream>

class Exception {
protected:
	std::stringstream error;
	
public:
	Exception();
	Exception(const Exception& ex);
	Exception(const std::string str);
	
	std::string what() const;
	
	template<class T>
	Exception& operator<<(const T& value) {
		this->error << value;
		return *this;
	}
};