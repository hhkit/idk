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
		void read_begin()
		{
			mu.lock();
			while (writer_waiting)
			{
				std::unique_lock<std::mutex> lock{ mu };
				cond.wait(lock);
			}
			readers_waiting++;
		}
		void read_end()
		{
			--readers_waiting;
			while (readers_waiting > 0)
			{
				std::unique_lock<std::mutex> lock{ mu };
				cond.wait(lock);
			}
			cond.notify_one();
			mu.unlock();
		}
		void write_begin()
		{
			mu.lock();
			while(writer_waiting)
			{
				std::unique_lock<std::mutex> lock{ mu };
				cond.wait(lock);
			}
		}
		void write_end()
		{
			writer_waiting = true;
			while (readers_waiting>0)
			{

				std::unique_lock<std::mutex> lock{ mu };
				cond.wait(lock);

			}
			writer_waiting = false;
			cond.notify_all();
			mu.unlock();
		}

		std::mutex mu = {};
		std::condition_variable cond = {};

		int readers_waiting= 0;
		bool writer_waiting = false;
	};
}