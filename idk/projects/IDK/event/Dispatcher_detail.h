#pragma once
#include <string_view>

namespace idk::detail
{
	template<typename T>
	struct storage_type
	{
		using type = T;
	};

	template<typename T>
	using storage_type_t = typename storage_type<T>::type;
}

#include "Dispatcher_detail.inl"