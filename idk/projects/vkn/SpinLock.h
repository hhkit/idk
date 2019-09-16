#pragma once

#include <atomic>
namespace idk
{
	struct SpinLock
	{
		SpinLock(std::atomic<bool>& lock) : _lock{ &lock }
		{
			bool expect = false;
			//Test and set, spin lock until ready
			while (expect = false, !_lock->compare_exchange_weak(expect, true))
			{
				//expect will be overwritten with the current value, so we rewrite our condition to false
				expect = false;
			}
		}
		SpinLock(SpinLock&& rhs) noexcept :_lock{ rhs._lock } { rhs._lock = nullptr; }
		~SpinLock()
		{
			//End of critical section, release order doesn't matter.
			_lock->store(false);
			_lock = nullptr;
		}
	private:
		std::atomic<bool>* _lock;
	};
}