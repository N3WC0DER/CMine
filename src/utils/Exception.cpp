#include "Exception.h"

Exception::Exception() {}

Exception::Exception(const Exception& ex) {
	this->error << ex.what();
}

Exception::Exception(const std::string str) {
	this->error << str;
}

std::string Exception::what() const {
	return this->error.str();
}