#include "stdafx.h"
#include <iostream>

#include "ThreadPool.h"
#include <mono/metadata/threads.h>
#include <script/ScriptSystem.h>
#include <parallel/circular_buffer.h>
//#include <Windows.h>

namespace idk::mt
{
	thread_local int _thread_id = 0;

	int thread_id()
	{
		return _thread_id;
	}

	void thread_main(ThreadPool* pool, int thid)
	{
		std::cout << "spawned thread " + std::to_string(thid) + "\n";
		_thread_id = thid;

		// busy wait
		while (Core::IsRunning())
			pool->ExecuteJob(thread_id());
	}

	ThreadPool::ThreadPool(const int thread_count)
	{
		const auto helper_thds = std::max(0, thread_count - 1);
		LOG_TO(LogPool::SYS, "spawning %d threads\n", helper_thds);
		//SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_ABOVE_NORMAL);
		
		for (int i = 1; i <= helper_thds; ++i)
		{
			threads.emplace_back(std::thread{ &thread_main, this, i });
		}
	}

	ThreadPool::~ThreadPool()
	{
		wait.notify_all();
		for (auto& elem : threads)
			elem.join();
	}

	void ThreadPool::ExecuteJob([[maybe_unused]] const int thid, bool wait_for_job)
	{
		if (wait_for_job)
		{
			std::unique_lock m{ lock };
			wait.wait(m, [this]()->bool { return job_count.load() != 0 || !Core::IsRunning(); }); 
		}

		auto job = jobs.pop_front();
		if (job)
		{
			--job_count;
			IDK_ASSERT(*job);
			(*job)();
		}		
	}
}