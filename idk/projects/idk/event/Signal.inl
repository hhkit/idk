#pragma once
#include "Signal.h"

namespace idk
{
	template<typename ... Params>
	template<typename Func>
	typename Signal<Params...>::SlotId Signal<Params...>::Listen(Func&& f)
	{
		_slots.emplace(_next_id, std::forward<Func>(f));
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
		for (auto& f : _slots)
			f.second(std::forward<Args>(args)...);
	}
}