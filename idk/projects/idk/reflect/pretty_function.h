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
#if _MSC_VER >= 1920
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

}