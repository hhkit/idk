#pragma once
#include <mutex>
#include <shared_mutex>

namespace idk
{
	template<typename T, void(T::* acquire)() , void(T::* release)() >
	struct guard
	{
		guard(T& lock) :_lock{ &lock }
		{
			(_lock->*acquire)();
		}
		guard(guard&& rhs) :_lock{ guard._lock }
		{
			rhs._lock = {};
		}
		guard& operator=(guard&& rhs)
		{
			std::swap(rhs._lock, _lock);
			return *this;
		}
		~guard()
		{
			if (_lock)
				(_lock->*release)();
		}
		T* _lock;
	};
	//reader biased.
	class raynal_rw_lock
	{
	public:
		void begin_read();
		void end_read();
		void begin_write();
		void end_write();
		using read_guard = guard<raynal_rw_lock,&raynal_rw_lock::begin_read, & raynal_rw_lock::end_read>;
		using write_guard = guard<raynal_rw_lock, &raynal_rw_lock::begin_write, & raynal_rw_lock::end_write>;
		read_guard lock_read() { return read_guard{ *this }; }
		write_guard lock_write() { return write_guard{ *this }; }
	private:
		std::mutex r{};
		std::shared_mutex g{};
		int b; 
	};

	// writer
	class rw_lock_wb
	{
	public:
		void begin_read();
		void end_read();
		void begin_write();
		void end_write();
		using read_guard = guard<rw_lock_wb, & rw_lock_wb::begin_read, & rw_lock_wb::end_read>;
		using write_guard = guard<rw_lock_wb, & rw_lock_wb::begin_write, & rw_lock_wb::end_write>;
	private:
		std::condition_variable cv;
		std::mutex global = {};
		int counter = 0;
		int num_readers_active{ 0};
		int num_writers_waiting = 0;
		bool writer_active = false;
	};
}