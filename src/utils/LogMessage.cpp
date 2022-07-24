#include "LogMessage.h"

LogMessage::LogMessage() {}

LogMessage::LogMessage(const LogMessage& msg) {
	this->buffer << msg.getMessage();
}

LogMessage::LogMessage(const std::string str) {
	this->buffer << str;
}

std::string LogMessage::getMessage() const {
	return this->buffer.str();
}