#pragma once
#include <thread>

template <typename... Args>
class AsyncTask {
private:
	std::thread* t;
	void(*f)(Args...);

public:
	AsyncTask(void(*f)(Args...), Args... args) 
			: f(f) {
		t = new std::thread(f, args...);
	}
	
	~AsyncTask() {
		t->join();
	}
};