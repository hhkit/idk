#pragma once

#include "idk.h"

namespace idk::reflect
{
	using ReflectedTypes = std::variant<
		int
		, bool
		, float
		, std::string
		, vec2
		, vec3
		, vec4
	>;
}