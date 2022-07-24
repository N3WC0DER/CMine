#pragma once
#include <vector>

#include "network/Socket.h"
#include "network/Session.h"
#include "network/utils/InternetAddress.h"
#include "network/utils/SocketException.h"
#include "utils/Logger.h"

class Socket; // циклическая зависимость
class Session; // циклическая зависимость

class SessionManager {
private:
	Socket* socket = nullptr;
	
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