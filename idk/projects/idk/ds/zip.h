#pragma once

namespace idk
{
	// note : zip can never store rvalue containers
	template<typename ... Containers>
	constexpr auto zip(Containers&...) noexcept;
}
#include "zip.inl"