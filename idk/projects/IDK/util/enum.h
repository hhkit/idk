#pragma once

#include <util/macro_utils.h>
#include <reflect/reflect.h>

// makes a reflectable enum that can be reflected, and converted to/from strings
// see https://stackoverflow.com/questions/28828957/enum-to-string-in-modern-c11-c14-c17-and-future-c20
// adapted to be more constexpr thanks to c++17 string_view.

// makes values_[] = { RED = 1, BLUE, GREEN } compileable
// by prepending (ignore_assign<UnderlyingType>) to each value.
// ie. (ignore_assign<UnderlyingType>)RED = 1
template <typename T>
struct ignore_assign {
	constexpr explicit ignore_assign(T val) : val{ val } {}
	constexpr operator T() const { return val; }
	constexpr const ignore_assign& operator=(T) const { return *this; }
	T val;
};

#define IGNORE_ASSIGN_SINGLE(e) (ignore_assign<UnderlyingType>)e,
#define IGNORE_ASSIGN(...) IDENTITY(FOREACH(IGNORE_ASSIGN_SINGLE, __VA_ARGS__))
#define TRIM_SINGLE(e) std::string_view{#e}.substr(0, std::string_view{#e}.find_first_of(" =")),
#define TRIM(...) IDENTITY(FOREACH(TRIM_SINGLE, __VA_ARGS__))

// make a struct that mimics an enum with better support. underlying_type should not be unsigned.
#define ENUM(name, underlying_type, ...)															\
struct name																							\
{																									\
	using UnderlyingType = underlying_type;															\
	enum Enum : UnderlyingType { __VA_ARGS__ };														\
	constexpr static size_t Count = IDENTITY(COUNT_ARGS(__VA_ARGS__));								\
	constexpr static UnderlyingType Values[] { IDENTITY(IGNORE_ASSIGN(__VA_ARGS__)) };				\
	constexpr static std::string_view Names[] { IDENTITY(TRIM(__VA_ARGS__)) };						\
																									\
	name() = delete;																				\
	constexpr name(Enum val)																		\
		: value{ val }																				\
	{																								\
		static_assert(std::is_integral_v<UnderlyingType> && !std::is_unsigned_v<UnderlyingType>);	\
	}																								\
	template<typename IntegralT>																	\
	constexpr name(IntegralT val)																	\
		: value { static_cast<UnderlyingType>(val) }												\
	{																								\
		static_assert(std::is_integral_v<IntegralT>);												\
	}																								\
	constexpr operator Enum() const { return (Enum)value; }											\
																									\
	constexpr std::string_view ToString() const														\
	{																								\
		for (size_t i = 0; i < Count; ++i)															\
		{																							\
			if (Values[i] == value)																	\
				return Names[i];																	\
		}																							\
		throw "enumerator not found";																\
	}																								\
																									\
	constexpr static name FromString(std::string_view str)											\
	{																								\
		for (size_t i = 0; i < Count; ++i)															\
		{																							\
			if (Names[i] == str)																	\
				return (Enum)Values[i];																\
		}																							\
		throw "enumerator not found";																\
	}																								\
																									\
private:																							\
	UnderlyingType value;																			\
	REFLECT_FRIEND																					\
};																									\
REFLECT_BEGIN(name, #name)																			\
REFLECT_CTOR(name::Enum)																			\
REFLECT_CTOR(int8_t)																				\
REFLECT_CTOR(int16_t)																				\
REFLECT_CTOR(int32_t)																				\
REFLECT_CTOR(int64_t)																				\
REFLECT_VAR(value)																					\
REFLECT_END()