#pragma once
#include <utility>

#define EXTENSION(x) static constexpr string_view ext = x;

namespace idk
{
	namespace detail {
		template<class>
		struct sfinae_true : std::true_type {};

		template<class T> static auto has_ext()->sfinae_true<decltype(T::ext)>;

		template<class> static auto has_ext(...)->std::false_type;
	} // detail::

	template <typename T>
	struct has_extension
		: decltype(detail::has_ext<T>())
	{};

	template<typename T>
	static constexpr auto has_extension_v = has_extension<T>::value;
}