#include "ThreadManager.h"

std::unique_ptr<ThreadManager> ThreadManager::instance = nullptr;

ThreadManager::ThreadManager(size_t threadCount) {
	this->enabled = true;
	
	for (int i = 0; i < threadCount; i++) {
		this->threads.push_back(std::make_unique<std::thread>(&ThreadManager::run, this));
	}
	
	Logger::getInstance()->info("ThreadManager started");
}

ThreadManager::~ThreadManager() {
	this->enabled = false;
	
	this->mutex_pool.lock();
	this->taskPool = std::queue<std::function<void ()>>();
	this->mutex_pool.unlock();
	
	for (auto thread = this->threads.begin(); thread != this->threads.end(); thread++) {
		this->cv_pool.notify_all();
		(*thread)->join();
		// TODO: Undefined behavior: thread not deleted
	}
	this->threads.clear();
	Logger::getInstance()->info("ThreadManager stopped");
}

void ThreadManager::init(size_t threadCount) {
	if (instance != nullptr)
			throw Exception() << "Error initialize ThreadManager";
	
	instance.reset(new ThreadManager(threadCount));
}

ThreadManager* ThreadManager::getInstance() {
	if (instance == nullptr)
			throw Exception() << "ThreadManager not initialize";
	
	return instance.get();
}

void ThreadManager::addToQueue(std::function<void ()> func) {
	std::scoped_lock<std::mutex> lock(this->mutex_pool);
	this->taskPool.push(func);
	
	this->cv_pool.notify_one();
}

void ThreadManager::run() {
	while (enabled) {
		std::unique_lock<std::mutex> lock(this->mutex_pool);
		
		this->cv_pool.wait(lock, [this] () -> bool { return !this->taskPool.empty() || !enabled; });
		
		if (this->taskPool.empty())
				continue;
		
		auto func = std::move(this->taskPool.front());
		this->taskPool.pop();
		lock.unlock();
		
		func();
	}
}