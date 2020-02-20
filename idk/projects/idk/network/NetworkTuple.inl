#pragma once

#include <tuple>
#include <reflect/typeinfo.h>

namespace idk::detail
{
	template<typename T> struct NetworkTuple;

	template<typename ... Ts>
	struct NetworkTuple<std::tuple<Ts...>>
	{
		static constexpr auto GenNames()
		{
			return std::array<string_view, sizeof...(Ts)>{
				reflect::fully_qualified_nameof<Ts>()...
			};
		}
	};
}