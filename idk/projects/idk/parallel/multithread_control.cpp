#include "multithread_control.h"

namespace idk
{
	void raynal_rw_lock::begin_read()
	{
		std::unique_lock lock{ r };
		++b;
		if (b == 1)
			g.lock_shared();
	}
	void raynal_rw_lock::end_read()
	{
		std::unique_lock lock{ r };
		--b;
		if (b == 0)
			g.unlock_shared();
	}
	void raynal_rw_lock::begin_write()
	{
		g.lock();
	}
	void raynal_rw_lock::end_write()
	{
		g.unlock();
	}

	void rw_lock_wb::begin_read()
	{
		std::unique_lock g{ global };
		while (num_writers_waiting > 0 || writer_active)
			cv.wait(g);
		++num_readers_active;
	}

	void rw_lock_wb::end_read()
	{
		std::unique_lock g{ global };
		--num_readers_active;
		if (num_readers_active == 0)
			cv.notify_one();
	}

	void rw_lock_wb::begin_write()
	{
		std::unique_lock g{ global };
		++num_writers_waiting;
		while (num_readers_active > 0 || writer_active)
			cv.wait(g);

		--num_writers_waiting;
		writer_active = true;
	}

	void rw_lock_wb::end_write()
	{
		std::unique_lock g{ global };
		writer_active = false;
		cv.notify_one();
	}
}