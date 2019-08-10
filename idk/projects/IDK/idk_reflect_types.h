#pragma once

#include "idk.h"

namespace idk::reflect
{
	// add reflected types here... only have to add
	// types that are used in other reflected types
	// ie. don't have to add "top-level" reflected types
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