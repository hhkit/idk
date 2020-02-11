#pragma once
#include <mutex>
#include <shared_mutex>

namespace idk
{
	//reader biased.
	class raynal_rw_lock
	{
	public:
		void begin_read();
		void end_read();
		void begin_write();
		void end_write();
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
	private:
		std::condition_variable cv;
		std::mutex global = {};
		int counter = 0;
		int num_readers_active{ 0};
		int num_writers_waiting = 0;
		bool writer_active = false;
	};
}