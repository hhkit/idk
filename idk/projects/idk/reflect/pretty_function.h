#pragma once

#if defined(__clang__)
	#define _IDK_PRETTY_FUNCTION __PRETTY_FUNCTION__
#elif defined(__GNUC__)
	#define _IDK_PRETTY_FUNCTION __PRETTY_FUNCTION__
#elif defined(_MSC_VER)
	#define _IDK_PRETTY_FUNCTION __FUNCSIG__
#else
	#error "No support for this compiler."
#endif

namespace idk::reflect::detail
{

	template<typename>
	constexpr string_view pretty_function() { return _IDK_PRETTY_FUNCTION; }
	template<template<typename...> typename>
	constexpr string_view pretty_function() { return _IDK_PRETTY_FUNCTION; }
	template<template<typename, auto> typename>
	constexpr string_view pretty_function() { return _IDK_PRETTY_FUNCTION; }
	template<template<auto...> typename>
	constexpr string_view pretty_function() { return _IDK_PRETTY_FUNCTION; }

	constexpr string_view extract_name_from_pretty_function(string_view sv)
	{
#if !defined(_MSC_VER) || _MSC_VER >= 1920
		sv.remove_prefix(sv.find("pretty_function<") + sizeof("pretty_function<") - 1);
		if (sv.find("class") != std::string_view::npos)
			sv.remove_prefix(sv.find("class") + sizeof("class"));
		else if (sv.find("struct") != std::string_view::npos)
			sv.remove_prefix(sv.find("struct") + sizeof("struct"));
		sv.remove_suffix(sizeof(">(void)") - 1);

#elif _MSC_VER >= 1911
		int off = sizeof("class std::basic_string_view<char,struct std::char_traits<char> > __cdecl idk::reflect::detail::pretty_function<") - 1;
		if (sv[off] == 'c', sv[off + 1] == 'l' && sv[off + 2] == 'a' && sv[off + 3] == 's' && sv[off + 4] == 's' && sv[off + 5] == ' ')
			off += sizeof("class");
		else if (sv[off] == 's' && sv[off + 1] == 't' && sv[off + 2] == 'r' && sv[off + 3] == 'u' && sv[off + 4] == 'c' && sv[off + 5] == 't' && sv[off + 6] == ' ')
			off += sizeof("struct");

		sv = std::string_view{
			sv.data() + off,
			sv.size() - off - sizeof(">(void)") + 1
		};
#endif

		return sv;
	}

	template<typename T>
	constexpr string_view pretty_function_name()
	{
		return extract_name_from_pretty_function(pretty_function<T>());
	}

	template<template<typename...> typename Tpl>
	constexpr string_view pretty_function_name()
	{
		return extract_name_from_pretty_function(pretty_function<Tpl>());
	}

	template<template<typename, auto> typename Tpl>
	constexpr string_view pretty_function_name()
	{
		return extract_name_from_pretty_function(pretty_function<Tpl>());
	}

	template<template<auto...> typename Tpl>
	constexpr string_view pretty_function_name()
	{
		return extract_name_from_pretty_function(pretty_function<Tpl>());
	}

}