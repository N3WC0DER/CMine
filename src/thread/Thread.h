#pragma once
#include <iostream>
#include <mutex>
#include <map>
#include <thread>
#include <chrono>

#include "../utils/Exception.h"

template <typename T>
class Thread {
private:
	pid_t pid = -1;
	T* cl = nullptr;
		
protected:
	std::thread* t = nullptr;
	
	virtual void onRun() = 0;
	virtual void onQuit() = 0;
	
public:
	Thread(T* cl) 
			: cl(cl) {}
	
	void startThread() {
		this->t = new std::thread([this] {
			this->pid = getpid();
			this->cl->onRun();
		});
	}
	
	void join() {
		this->t->join();
	}
	
	void wait(unsigned int time) {
		auto start = std::chrono::high_resolution_clock::now();
		auto end = start + std::chrono::microseconds(time);
		do {
			this->t->yield();
		} while (std::chrono::high_resolution_clock::now() < end);
	}
	
	void sleep(unsigned int time) {
		this->t->sleep_for(time);
	}
	
	void stopThread() {
		kill(this->pid, 0);
		this->cl->onQuit();
	}
	
	~Thread() {
		kill(this->pid, 0);
	}
};