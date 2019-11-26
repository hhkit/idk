#include <future>
#include <functional>
#include <thread>
#include <tuple>
#include <utility>

#include <core/ISystem.h>
#include "queue.h"

namespace idk::mt
{
	extern thread_local int thread_id;

	class ThreadPool
	{
	public:
		template<typename T>
		struct Future;

		ThreadPool(const int threads = std::thread::hardware_concurrency());
		~ThreadPool();
		
		template<typename T, typename ... Args>
		auto Post(T&& func, Args&& ... args);

		void ExecuteJob(const int thid);
	private:
		std::vector<std::thread> threads;
		queue<std::function<void()>> jobs;
	};
	
	template<typename T, typename ... Args>
	auto ThreadPool::Post(T&& func, Args&& ... args)
	{
		using Retval = decltype(std::apply(func, std::declval<std::tuple<Args...>>()));
		auto promise = std::make_shared<std::promise<Retval>>();
		auto future = promise->get_future();
		
		jobs.emplace_back(mt::thread_id, 
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
		
		return future;
	}
}