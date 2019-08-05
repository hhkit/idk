#pragma once

namespace idk
{
	template<typename ... Containers>
	auto zip(Containers&&...);
}
#include "zip.inl"