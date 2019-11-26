#include <future>
#include <functional>
#include <thread>
#include <tuple>
#include <utility>

#include <core/ISystem.h>
#include "queue.h"

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

		void ExecuteJob(const int thid);
	private:
		std::vector<std::thread> threads;
		queue<std::function<void()>> jobs;
	};

	template<typename T>
	class ThreadPool::Future
	{
	public:
		Future() = default;
		Future(ThreadPool* in_pool, std::future<T>&& in_fut) : pool{ in_pool }, future{ std::move(in_fut) }{}
		T get();
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
		
		jobs.emplace_back(mt::thread_id(), 
			[ fn = func
			, tuple = std::make_tuple(std::forward<Args>(args)...)
			, promise
			]() -> void
		{
			if constexpr (std::is_same_v<Retval, void>)
			{
				std::apply(fn, tuple);
				promise->set_value();
			}
			else
				promise->set_value(std::apply(fn, tuple));
		});
		
		return Future{ this, std::move(future) };
	}
	template<typename T>
	T ThreadPool::Future<T>::get()
	{
		IDK_ASSERT(pool);
		while (future.wait_for(std::chrono::seconds{}) != std::future_status::ready)
			pool->ExecuteJob(thread_id());
		return future.get();
	}

	template<typename T>
	using Future = ThreadPool::Future<T>;
}