#pragma once
#include <idk.h>
#include <core/Handle.h>
#include <mono/jit/jit.h>

namespace idk
{
	template<typename T>
	decltype(auto) decay(T&& arg)
	{
		return +arg;
	}

	template<typename Type, typename Mem>
	void BindGetter(string_view symbol, Mem (Type::* mem_fn)() const)
	{
		if constexpr (ComponentID<Type> != ComponentCount)
		{
			mono_add_internal_call(symbol, decay([](Handle<Type> h) -> std::decay_t<Mem>
				{
					return std::invoke(mem_fn, *h);
				}));
		}
	}


	//template<typename Type, typename Mem>
	//void BindGetter(string_view, void (Type::* mem_fn)(Mem))
	//{
	//
	//}
}