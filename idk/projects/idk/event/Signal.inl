#pragma once
#include "Signal.h"

namespace idk
{
	template<typename ... Params>
	template<typename Func>
	typename Signal<Params...>::SlotId Signal<Params...>::Listen(Func&& f, int fire_count)
	{
		_slots.emplace(_next_id, Callback{ std::forward<Func>(f), fire_count });
		return _next_id++;
	}
	template<typename ...Params>
	inline void Signal<Params...>::Unlisten(SlotId id)
	{
		_removeus.push_back(id);
	}

	template<typename ... Params>
	template<typename ... Args>
	void Signal<Params...>::Fire(Args&& ... args)
	{
		for (auto& elem : _removeus)
			_slots.erase(elem);
		_removeus.clear();

		for (auto& [slot, callback] : _slots)
		{
			callback.func(std::forward<Args>(args)...);
			if (callback.callback_count > 0)
			{
				if (--callback.callback_count == 0)
					_removeus.emplace_back(slot);
			}
		}
	}
}