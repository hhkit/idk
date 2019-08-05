#pragma once
#include <idk_config.h>
#include <tuple>

namespace idk
{
	using Handleables = std::decay_t<decltype(std::tuple_cat(
		std::declval<std::tuple<class GameObject>>(),
		std::declval<Components>()
	))>;

	static constexpr auto HandleableCount = std::tuple_size_v<Handleables>;
}