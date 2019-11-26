#include "stdafx.h"
#include "ThreadPool.h"
#include <iostream>
namespace idk::mt
{
	thread_local int thread_id = 0;

	void thread_main(ThreadPool* pool, int thid)
	{
		std::cout << "spawned thread " + std::to_string(thid) + "\n";
		thread_id = thid;

		// busy wait
		while (Core::IsRunning())
			pool->ExecuteJob(thread_id);
	}

	ThreadPool::ThreadPool(const int thread_count)
	{
		LOG_TO(LogPool::SYS, "spawning %d threads\n", thread_count);
		//int i = 1;
		for (int i = 1; i <= std::max(0, thread_count); ++i)
			threads.emplace_back(std::thread{ &thread_main, this, i });
	}

	ThreadPool::~ThreadPool()
	{
		for (auto& elem : threads)
			elem.join();
	}

	void ThreadPool::ExecuteJob(const int thid)
	{
		static int i = 0;
		auto job = jobs.pop_front(thid);
		if (job)
		{
			++i;
			IDK_ASSERT(*job);
			(*job)();
		}
	}
}