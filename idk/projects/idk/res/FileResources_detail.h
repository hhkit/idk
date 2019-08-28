#pragma once
#include <variant>
#include <tuple>

#include <idk_config.h>
#include <res/ResourceHandle.h>

namespace idk::detail
{
	template<typename T>
	struct ResourceFile_helper;

	template<typename ... Ts>
	struct ResourceFile_helper<std::tuple<Ts...>>
	{
		using GenericRscHandle = std::variant<RscHandle<Ts>...>;
	};
}