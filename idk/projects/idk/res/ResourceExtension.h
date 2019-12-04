#pragma once
#include <utility>

#define EXTENSION(EXT) \
static constexpr string_view ext = EXT; \
static_assert(ext[0] == '.', "Extensions must start with a period!");

namespace idk
{
	namespace detail
	{
		template<class> struct sfinae_true : std::true_type {};

		template<class T> static auto has_ext()->sfinae_true<decltype(T::ext)>;
		template<class> static auto has_ext(...) -> std::false_type;
	}

	template<typename T>
	struct has_extension
		: decltype(detail::has_ext<T>())
	{};

	template<typename T>
	static constexpr auto has_extension_v = has_extension<T>::value;


}