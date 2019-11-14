#pragma once
#include <mutex>

namespace idk
{
	//reader biased.
	struct raynal_rw_lock
	{

		void begin_read()
		{
			reader_lock.lock();
			++counter;
			if (counter == 1)
				global.lock();
			reader_lock.unlock();
		}
		void end_read()
		{
			reader_lock.lock();
			--counter;
			if (counter == 0)
				global.unlock();
			reader_lock.unlock();
		}
		void begin_write()
		{
			global.lock();
		}
		void end_write()
		{
			global.unlock();
		}


		std::mutex global = {}, reader_lock={};
		int counter = 0;
	};

	//writer biased
	struct rw_lock_wb
	{
		using lock_t =std::unique_lock<std::mutex>;
		auto read_begin()
		{
			lock_t lock{ mu };
			while (writer_waiting)
			{
				cond.wait(lock);
			}
			readers_waiting++;
			return std::move(lock);
		}
		void read_end(lock_t& lock)
		{
			--readers_waiting;
			while (readers_waiting > 0)
			{
				cond.wait(lock);
			}
			cond.notify_one();
			lock.unlock();
		}
		auto write_begin()
		{
			lock_t lock{ mu };
			while(writer_waiting)
			{
				cond.wait(lock);
			}
			return std::move(lock);
		}
		void write_end(lock_t& lock)
		{
			writer_waiting = true;
			while (readers_waiting>0)
			{
				cond.wait(lock);
			}
			writer_waiting = false;
			cond.notify_all();
			lock.unlock();
		}

		std::mutex mu = {};
		std::condition_variable cond = {};

		int readers_waiting= 0;
		bool writer_waiting = false;
	};
}