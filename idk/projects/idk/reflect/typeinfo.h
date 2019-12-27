#pragma once

#include <idk.h>
#include <util/string_hash.inl>
#include <reflect/detail/pretty_function.h>

namespace idk::reflect
{
	// get full qualified type name of T (decayed).
	// eg. vec3& => idk::tvec<float, 3>
	// NOTE: if comparing types, use typehash<T>() !!!
	template<typename T> 
	constexpr string_view fully_qualified_nameof()
	{
		return detail::pretty_function_name<std::decay_t<T>>(); 
	}
	template<template<typename... > typename Tpl>
	constexpr string_view fully_qualified_nameof()
	{ 
		return detail::pretty_function_name<Tpl>(); 
	}
	template<template<typename, auto> typename Tpl> 
	constexpr string_view fully_qualified_nameof() 
	{
		return detail::pretty_function_name<Tpl>();
	}
	template<template<auto... > typename Tpl> 
	constexpr string_view fully_qualified_nameof()
	{
		return detail::pretty_function_name<Tpl>(); 
	}

	// gets hash of type T (decayed).
	// use this against type.hash()
	template<typename T> 
	constexpr size_t typehash() 
	{ 
		return idk::string_hash(fully_qualified_nameof<T>()); 
	}
}