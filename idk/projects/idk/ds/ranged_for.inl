#pragma once
#include <utility>
#include "ranged_for.h"
namespace idk
{
	template<typename Container>
	constexpr auto reverse(Container&& cont) noexcept
	{
		return range_over<decltype(std::rbegin(std::declval<Container>())), Container>{cont, std::rbegin(cont), std::rend(cont)};
	}
}