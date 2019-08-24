#pragma once

#include <reflect/reflect.h>

namespace idk::reflect
{
	// construct an instance of this type
	template<typename... Ts>
	dynamic type::create(Ts&& ... args) const
	{
		return _context->construct(dynamic{ std::forward<Ts>(args) }...);
	}

	template<typename T>
	bool type::is() const
	{
		return hash() == typehash<T>();
	}

	// Checks if this type is a template type Tpl<typename...>
	// Please use the fully qualified template name
	template<template<typename...> typename Tpl> bool type::is_template() const
	{
		constexpr auto sv = fully_qualified_nameof<Tpl>();
		return name().find(sv) == 0 && name()[sv.size()] == '<';
	}

	// Checks if this type is a template type Tpl<typename, auto>
	// Please use the fully qualified template name
	template<template<typename, auto> typename Tpl> bool type::is_template() const
	{
		constexpr auto sv = fully_qualified_nameof<Tpl>();
		return name().find(sv) == 0 && name()[sv.size()] == '<';
	}

	// Checks if this type is a template type Tpl<auto...>
	// Please use the fully qualified template name
	template<template<auto...> typename Tpl> bool type::is_template() const
	{
		constexpr auto sv = fully_qualified_nameof<Tpl>();
		return name().find(sv) == 0 && name()[sv.size()] == '<';
	}

}