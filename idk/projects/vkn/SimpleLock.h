#pragma once
#include <idk.h>
#include <mutex>
namespace idk::vkn::hlp
{
	struct SimpleLock
	{
	public:
		SimpleLock() :_data{ std::make_unique<Data>() } {}
		SimpleLock(SimpleLock&&)noexcept = default;
		SimpleLock(const SimpleLock&) = delete;
		SimpleLock& operator=(SimpleLock&&)noexcept = default;
		SimpleLock& operator=(const SimpleLock&) = delete;
		std::mutex& Mutex() { _data->mutex; }
		std::condition_variable& Cv() { _data->cv; }
		void Lock()const;
		void Unlock()const;
		~SimpleLock() = default;
	private:
		struct Data
		{
			std::mutex mutex;
			std::condition_variable cv;
			bool locked = false;
		};
		unique_ptr<Data> _data;
	};
}