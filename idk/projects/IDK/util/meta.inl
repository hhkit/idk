#pragma once
#include <type_traits>

namespace idk
{
	template<typename T, template<typename ...> typename TMPLT>
	struct is_template
		: std::false_type
	{
	};

	template<template<typename ...> typename TMPLT, typename...Args>
	struct is_template<TMPLT<Args...>, TMPLT>
		: std::true_type
	{
	};
}