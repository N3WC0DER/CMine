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
		if (MTU > MAX_MTU_SIZE || MTU < MIN_MTU_SIZE)
				throw SocketException() << "Bad MTU: " << MTU;
		
		std::scoped_lock<std::mutex> lock(this->mutex);
		auto temp = std::make_unique<Session>(this->socket, addr, MTU, GUID);
		this->sessions.push_back(std::move(temp));
		Logger::getInstance()->debug(LogMessage() << "Session {" << addr->toString() << "} created.");
	} catch (const SocketException& ex) {
		Logger::getInstance()->warning(LogMessage() << "Session {" << addr->toString() << "} not created. " << ex.what());
	}
}

Session* SessionManager::getSession(const InternetAddress* addr) {
	std::scoped_lock<std::mutex> lock(this->mutex);
	for (auto session = this->sessions.begin(); session != this->sessions.end(); session++) {
		if (*((*session)->getAddress()) == *addr)
				return (*session).get();
	}
	return nullptr;
}

Session* SessionManager::getSession(const int64_t GUID) {
	std::scoped_lock<std::mutex> lock(this->mutex);
	for (auto session = this->sessions.begin(); session != this->sessions.end(); session++) {
		if ((*session)->getGUID() == GUID)
				return (*session).get();
	}
	return nullptr;
}

void SessionManager::closeSession(const Session* session) {
	auto i = std::find_if(this->sessions.begin(), this->sessions.end(), [session] (const auto& value) {
		if (value.get() == session) return true;
		return false;
	});
	if (i == this->sessions.end()) throw SocketException() << "Session {" << session->getAddress()->toString() << "} not closed: IP address not found";
	this->sessions.erase(i);
}

void SessionManager::closeSession(const InternetAddress* addr) {
	std::scoped_lock<std::mutex> lock(this->mutex);
	int size = this->sessions.size();
	for (auto session = this->sessions.begin(); session != this->sessions.end(); session++) {
		if (*((*session)->getAddress()) == *addr) {
			(*session).reset();
			this->sessions.erase(session);
			Logger::getInstance()->debug(LogMessage() << "Session {" << addr->toString() << "} closed");
			break;
		}
	}
	if (size == this->sessions.size())
			throw SocketException() << "Session {" << addr->toString() << "} not closed: IP address not found";
}

void SessionManager::closeSession(const int64_t GUID) {
	std::scoped_lock<std::mutex> lock(this->mutex);
	int size = this->sessions.size();
	for (auto session = this->sessions.begin(); session != this->sessions.end(); session++) {
		if ((*session)->getGUID() == GUID) {
			(*session).reset();
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
		this->mutex.lock();
		
		if (this->sessions.empty()) {
			this->mutex.unlock();
			std::this_thread::yield();
			continue;
		}
		
		for (auto session = this->sessions.begin(); session != this->sessions.end(); session++) {
			(*session)->update();
		}
		this->mutex.unlock();
	}
}