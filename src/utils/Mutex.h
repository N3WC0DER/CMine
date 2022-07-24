#pragma once
#include <mutex>

/** Чтобы упростить управление мьютексами, перенес их в отдельный файл */
namespace Mutex {
	static std::mutex serverState;
	//static std::mutex sessionManagerShutdown;
}