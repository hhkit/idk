#include <thread>
#include <functional>
#include "queue.h"

namespace idk::mt
{
	class ThreadPool
	{
	public:
		ThreadPool(const int threads = std::thread::hardware_concurrency);
		~ThreadPool();
		
		template<typename T>
		auto Post(T&& func);
	private:
		std::vector<std::thread> threads;
		queue<std::function<void(int)>> jobs;
	};
}