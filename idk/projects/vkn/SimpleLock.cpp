#include "pch.h"
#include "SimpleLock.h"

namespace idk::vkn::hlp
{
	/*
	template<typename Tuple, typename Pack>
	struct Forwarder;
	template<typename Tuple,template<typename...> typename Pack, typename ...Args>
	struct Forwarder<Tuple, Pack<Args...>>
	{
		Tuple storage;
		using pack_t = Pack<Args...>;
		template<typename Func,size_t...Sequence>
		auto apply(Func&& func,std::integer_sequence<size_t,Sequence...> seq)
		{
			return func(std::forward<Args>(std::get<Sequence>(storage))...);
		}
	};
	*/

	void SimpleLock::Lock()const
	{
		std::unique_lock lock{ _data->mutex };
		while (_data->locked)
			_data->cv.wait(lock);
		_data->locked = true;
	}

	void SimpleLock::Unlock()const
	{
		_data->locked = false;
		_data->cv.notify_one();
	}


	SimpleLockGuard::SimpleLockGuard(SimpleLock& lock):_lock{&lock}
	{
		lock.Lock();
	}

	SimpleLockGuard::SimpleLockGuard(SimpleLockGuard&& other):_lock{other._lock}
	{
		other._lock = nullptr;
	}

	SimpleLockGuard::~SimpleLockGuard()
	{
		if(_lock)
			_lock->Unlock();
		_lock = nullptr;
	}

}