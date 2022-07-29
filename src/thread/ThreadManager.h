#pragma once
#include <iostream>
#include <queue>
#include <vector>
#include <thread>
#include <future>

#include "utils/Logger.h"
#include "utils/Exception.h"

class ThreadManager {
private:
	ThreadManager(size_t threadCount);
	ThreadManager(const ThreadManager&) = delete;
	ThreadManager& operator=(const ThreadManager&) = delete;
	
	std::atomic<bool> enabled;
	
	std::queue<std::function<void ()>> taskPool;
	std::mutex mutex_pool;
	std::condition_variable cv_pool;
	
	std::vector<std::unique_ptr<std::thread>> threads;
	
	static std::unique_ptr<ThreadManager> instance;
	
	void addToQueue(std::function<void ()> func);
	
	void run();
	
public:
	~ThreadManager();
	
	static void init(size_t threadCount = std::thread::hardware_concurrency());
	static ThreadManager* getInstance();
	
	template <typename Func, typename... Args>
	auto addTask (Func&& func, Args&&... args) -> std::future<std::invoke_result_t<Func, Args...>> {
	  using returnType = std::invoke_result_t<Func, Args...>;
	  auto task = std::make_shared<std::packaged_task<returnType()>>(
	              std::bind(std::forward<Func>(func), std::forward<Args>(args)...));
	
	  this->addToQueue([task] () { (*task)(); });
	
	  return task->get_future();
	}
};