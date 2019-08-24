#pragma once

#include <util/macro_utils.h>

// makes a reflectable enum that can be reflected, and converted to/from strings
// see https://stackoverflow.com/questions/28828957/enum-to-string-in-modern-c11-c14-c17-and-future-c20
// adapted to be more constexpr thanks to c++17 string_view.

// makes values_[] = { RED = 1, BLUE, GREEN } compileable
// by prepending (ignore_assign<UnderlyingType>) to each value.
// ie. (ignore_assign<UnderlyingType>)RED = 1
namespace idk::detail
{
	template <typename T>
	struct ignore_assign {
		constexpr explicit ignore_assign(T val) : val{ val } {}
		constexpr operator T() const { return val; }
		constexpr const ignore_assign& operator=(T) const { return *this; }
		T val;
	};
}

#define IGNORE_ASSIGN_SINGLE(e) (idk::detail::ignore_assign<UnderlyingType>)e,
#define IGNORE_ASSIGN(...) IDENTITY(FOREACH(IGNORE_ASSIGN_SINGLE, __VA_ARGS__))
#define TRIM_SINGLE(e) std::string_view{#e}.substr(0, std::string_view{#e}.find_first_of(" =")),
#define TRIM(...) IDENTITY(FOREACH(TRIM_SINGLE, __VA_ARGS__))

// make a struct that mimics an enum with better support.
// underlying_type should not be unsigned.
// use REFLECT_ENUM macro to register enum.
#define ENUM(name, underlying_type, ...)															\
struct name																							\
{																									\
	using UnderlyingType = underlying_type;															\
	enum _enum : UnderlyingType { __VA_ARGS__ };													\
	constexpr static size_t count = IDENTITY(COUNT_ARGS(__VA_ARGS__));								\
	constexpr static UnderlyingType values[] { IDENTITY(IGNORE_ASSIGN(__VA_ARGS__)) };				\
	constexpr static idk::string_view names[] { IDENTITY(TRIM(__VA_ARGS__)) };						\
																									\
	name() = delete;																				\
	constexpr name(_enum val)																		\
		: value{ val }																				\
	{																								\
		static_assert(std::is_integral_v<UnderlyingType> && !std::is_unsigned_v<UnderlyingType>);	\
	}																								\
	template<typename IntegralT, typename = std::enable_if_t<std::is_integral_v<IntegralT>>>		\
	constexpr name(IntegralT val)																	\
		: value { static_cast<UnderlyingType>(val) }												\
	{																								\
	}																								\
	constexpr operator _enum() const { return (_enum)value; }										\
	explicit operator string() const { return idk::string(to_string()); }							\
																									\
	constexpr idk::string_view to_string() const													\
	{																								\
		for (size_t i = 0; i < count; ++i)															\
		{																							\
			if (values[i] == value)																	\
				return names[i];																	\
		}																							\
		throw "enumerator not found";																\
	}																								\
																									\
	constexpr static name from_string(idk::string_view str)											\
	{																								\
		for (size_t i = 0; i < count; ++i)															\
		{																							\
			if (names[i] == str)																	\
				return (_enum)values[i];															\
		}																							\
		throw "enumerator not found";																\
	}																								\
																									\
private:																							\
	UnderlyingType value;																			\
	template<typename> friend struct ::property::opin::def;											\
};
