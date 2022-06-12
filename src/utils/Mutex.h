#pragma once
#include <mutex>

/** Чтобы упростить управление мьютексами, перенес их в отдельный файл */
namespace Mutex {
	static std::mutex recvSocket;
	static std::mutex handlePacket;
	static std::mutex stopHandler;
}