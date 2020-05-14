#pragma once
#include <future>
#include <functional>
#include <thread>
#include <tuple>
#include <utility>

#include <core/ISystem.h>
#include "circular_buffer.h"

namespace idk::mt
{
	int thread_id();

	class ThreadPool
	{
	public:
		template<typename T>
		class Future;

		ThreadPool(const int threads = std::thread::hardware_concurrency());
		~ThreadPool();
		
		template<typename T, typename ... Args>
		auto Post(T&& func, Args&& ... args);

		void ExecuteJob(const int thid, bool wait_for_job = true);
	private:
		std::vector<std::thread> threads;

		std::mutex lock; // this lock exists for the condition variable
		std::condition_variable wait; // signals all threads to check the job_count
		atomic<unsigned> job_count{0};
		circular_buffer_lf<std::function<void()>> jobs;
	};

	template<typename T>
	class ThreadPool::Future
	{
	public:
		Future() = default;
		Future(ThreadPool* in_pool, std::future<T>&& in_fut) : pool{ in_pool }, future{ std::move(in_fut) }{}
		T get();
		bool ready();
	private:
		ThreadPool* pool = nullptr;
		std::future<T> future;
	};
	
	template<typename T, typename ... Args>
	auto ThreadPool::Post(T&& func, Args&& ... args)
	{
		using Retval = decltype(std::apply(func, std::declval<std::tuple<Args...>>()));
		auto promise = std::make_shared<std::promise<Retval>>();
		auto future = promise->get_future();

		auto job = [fn = func
			, tuple = std::make_tuple(std::forward<Args>(args)...)
			, promise
		]() -> void
		{
			try
			{

			if constexpr (std::is_same_v<Retval, void>)
			{
				std::apply(fn, tuple);
				promise->set_value();
			}
			else
				promise->set_value(std::apply(fn, tuple));
			}
			catch (...)
			{
				promise->set_exception(std::current_exception());
			}
		};
		
		while (!jobs.emplace_back(job))
			ExecuteJob(thread_id(), thread_id() != 0);

		++job_count;
		wait.notify_one();
		return Future{ this, std::move(future) };
	}
	template<typename T>
	T ThreadPool::Future<T>::get()
	{
		IDK_ASSERT(pool);
		//auto future_is_ready = [](auto& future)
		//{
		//	auto status = future.wait_for(std::chrono::nanoseconds{ 5 });
		//	return status == std::future_status::ready;
		//};
		//while (!future_is_ready(future))
		while (future.wait_for(std::chrono::nanoseconds{5}) != std::future_status::ready)
			pool->ExecuteJob(thread_id(), thread_id() != 0);
		return future.get();
	}

	template<typename T>
	bool ThreadPool::Future<T>::ready()
	{
		return future.wait_for(std::chrono::nanoseconds{ 0 }) == std::future_status::ready;
	}

	template<typename T>
	using Future = ThreadPool::Future<T>;
}