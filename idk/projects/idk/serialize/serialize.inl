#pragma once

#include "serialize.h"

namespace idk
{

	// true if is integral/floating point, or is convertible to string
	template<typename T, typename = void>
	struct is_basic_serializable : std::is_arithmetic<T> {};

	// true if is integral/floating point, or is convertible to string
	template<typename T>
	struct is_basic_serializable<T, std::void_t<decltype(string(std::declval<T>()))>> : std::true_type {};

	template<typename T>
	string serialize_text(const T& obj)
	{
		if constexpr (std::is_arithmetic_v<T>)
			return std::to_string(obj);
		else if constexpr (is_basic_serializable_v<T>)
			return string(obj);
		else
			return serialize_text(reflect::dynamic{ obj });
	}

	template<typename T>
	void parse_text(const string& str, T& obj)
	{
		if constexpr (std::is_same_v<std::decay_t<T>, int>)
			obj = std::stoi(str);
		else if constexpr (std::is_same_v<std::decay_t<T>, bool>)
			obj = str == "true";
		else if constexpr (std::is_same_v<std::decay_t<T>, char>)
			obj = str[0];
		else if constexpr (std::is_same_v<std::decay_t<T>, int64_t>)
			obj = std::stoll(str);
		else if constexpr (std::is_same_v<std::decay_t<T>, uint64_t>)
			obj = std::stoull(str);
		else if constexpr (std::is_same_v<std::decay_t<T>, float>)
			obj = std::stof(str);
		else if constexpr (std::is_same_v<std::decay_t<T>, double>)
			obj = std::stod(str);
		else if constexpr (is_basic_serializable_v<T>)
			obj = T(str);
		else
			obj = parse_text(str, reflect::get_type<T>()).get<T>();
	}

	template<typename T>
	T parse_text(const string& str)
	{
		T obj;
		parse_text(str, obj);
		return obj;
	}

}