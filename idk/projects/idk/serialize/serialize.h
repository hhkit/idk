#pragma once

#include <idk.h>

namespace idk
{
	// true if is integral/floating point, or is convertible to string
	template<typename T, typename = void>
	struct is_basic_serializable : std::is_arithmetic<T> {};
	// true if is integral/floating point, or is convertible to string
	template<typename T>
	struct is_basic_serializable<T, std::void_t<decltype(string(std::declval<T>()))>> : std::true_type {};

	namespace reflect { class dynamic; }

	template<typename T>
	string serialize_text(const T& obj)
	{
		if constexpr(std::is_arithmetic_v<T>)
			return std::to_string(obj);
		else if constexpr (is_basic_serializable<T>::value)
			return string( obj );
		else
			return serialize_text(reflect::dynamic{ obj });
	}

	template<>
	string serialize_text(const reflect::dynamic& obj);

}