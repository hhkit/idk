#pragma once

namespace idk
{
	template<typename ... Containers>
	constexpr auto zip(Containers&&...) noexcept;
}
#include "zip.inl"