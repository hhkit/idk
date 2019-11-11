#pragma once
#include <idk.h>
#include <core/Handle.h>
#include <mono/jit/jit.h>

namespace idk
{
	template<typename Ret, typename ... Args>
	auto to_cdecl(Ret(*func)(Args...))
	{
		using Func = Ret(*)(Args...);
		return s_cast<Func>(func);
	}

	template<typename T>
	decltype(auto) decay(T&& arg)
	{
		return to_cdecl(+arg);
	}


	//template<typename Type, typename Mem>
	//void BindGetter(string_view, void (Type::* mem_fn)(Mem))
	//{
	//
	//}
}