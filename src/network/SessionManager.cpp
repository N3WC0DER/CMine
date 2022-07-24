#include "SessionManager.h"

SessionManager::SessionManager(Socket* socket)
		: socket(socket) {
	Logger::getInstance()->debug("SessionManager started");
}

SessionManager::~SessionManager() {
	Logger::getInstance()->debug("SessionManager stopped");
}

void SessionManager::createSession(const InternetAddress* addr, const uint16_t MTU, const int64_t GUID) {
	try {
		this->sessions.push_back(std::make_unique<Session>(this->socket, addr, MTU, GUID));
		Logger::getInstance()->notice(LogMessage() << "Session {" << addr->toString() << "} created.");
	} catch (const SocketException& ex) {
		Logger::getInstance()->warning(LogMessage() << "Session {" << addr->toString() << "} not created. " << ex.what());
	}
}

Session* SessionManager::getSession(const InternetAddress* addr) {
	for (auto session : this->sessions) {
		if (*(session->getAddress()) == *addr)
				return session;
	}
	return nullptr;
}

Session* SessionManager::getSession(const int64_t GUID) {
	for (auto session : this->sessions) {
		if (session->getGUID() == GUID)
				return session;
	}
	return nullptr;
}

void SessionManager::closeSession(const Session* session) {
	this->closeSession(session->getAddress());
}

void SessionManager::closeSession(const InternetAddress* addr) {
	int size = this->sessions.size();
	for (auto session = this->sessions.begin(); session != this->sessions.end(); session++) {
		if (*((*session)->getAddress()) == *addr) {
			(*session).reset(nullptr);
			this->sessions.erase(session);
			Logger::getInstance()->debug(LogMessage() << "Session {" << addr->toString() << "} closed");
			break;
		}
	}
	if (size == this->sessions.size())
			throw SocketException() << "Session {" << addr->toString() << "} not closed: IP address not found";
}

void SessionManager::closeSession(const int64_t GUID) {
	int size = this->sessions.size();
	for (auto session = this->sessions.begin(); session != this->sessions.end(); session++) {
		if ((*session)->getGUID() == GUID) {
			(*session).reset(nullptr);
			this->sessions.erase(session);
			Logger::getInstance()->debug(LogMessage() << "Session {" << (*session)->getAddress()->toString() << "} closed");
			break;
		}
	}
	if (size == this->sessions.size())
			throw SocketException() << "Session {" << GUID << "} not closed: GUID not found";
}

void SessionManager::updateSessions() {
	while (!this->socket->getServer()->isShutdown()) {
		for (auto session : this->sessions) {
			session->update();
		}
	}
}