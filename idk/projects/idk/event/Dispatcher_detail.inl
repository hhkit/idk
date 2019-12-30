#pragma once

#include "Dispatcher_detail.h"

namespace idk::detail
{
	template<typename T>
	struct storage_type<const T>
	{
		using type = storage_type_t<T>;
	};

	template<typename T>
	struct storage_type<const T&>
	{
		using type = storage_type_t<T>;
	};

	template<typename T>
	struct storage_type<T&>
	{
		using type = T &;
	};

	template<typename T>
	struct storage_type<T&&>
	{
		using type = storage_type_t<T>;
	};

	template<>
	struct storage_type<std::string_view>
	{
		using type = std::string;
	};
}