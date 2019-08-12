#pragma once
#include <string_view>

namespace idk::reflect::detail
{

	template<typename T>
	constexpr std::string_view pretty_function() { return __FUNCSIG__; }

	template<typename T>
	constexpr std::string_view pretty_function_name()
	{
		std::string_view sv{ pretty_function<T>() };
		sv.remove_prefix(sv.find("pretty_function<") + sizeof("pretty_function<") - 1);
		if (sv.find("class") != std::string_view::npos)
			sv.remove_prefix(sv.find("class") + sizeof("class"));
		else if (sv.find("struct") != std::string_view::npos)
			sv.remove_prefix(sv.find("struct") + sizeof("struct"));
		sv.remove_suffix(sizeof(">(void)") - 1);
		return sv;
	}

}