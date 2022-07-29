#pragma once
#include <vector>

#include "network/Socket.h"
#include "network/Session.h"
#include "network/utils/InternetAddress.h"
#include "network/utils/SocketException.h"
#include "utils/Logger.h"

class Socket; // cyclic dependency
class Session; // cyclic dependency

class SessionManager {
private:
	constexpr uint16_t MIN_MTU_SIZE = 576;
	constexpr uint16_t MAX_MTU_SIZE = 1500;
	Socket* socket = nullptr;
	
	mutable std::mutex mutex;
	
	std::vector<std::unique_ptr<Session>> sessions;
	
public:
	SessionManager(Socket* socket);
	~SessionManager();
	
	void createSession(const InternetAddress* addr, const uint16_t MTU, const int64_t GUID);
	
	Session* getSession(const InternetAddress* addr);
	Session* getSession(const int64_t GUID);
	
	void closeSession(const Session* session);
	void closeSession(const InternetAddress* addr);
	void closeSession(const int64_t GUID);
	
	void updateSessions();
};