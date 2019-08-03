#pragma once
#include <type_traits>

namespace idk
{
	template<typename T, template<typename ...> typename Template>
	struct is_template;
}

#include "meta.inl"