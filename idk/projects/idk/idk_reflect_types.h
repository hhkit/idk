#pragma once

#include "idk.h"
#include <core/Handle.h>
#include <res/Guid.h>

namespace idk
{
	class GameObject;
}

namespace idk::reflect
{

	// add reflected types here... only have to add
	// types that are used in other reflected types
	// ie. don't have to add "top-level" reflected types
	using ReflectedTypes = std::variant<
		int
		, bool
		, char
		, int64_t
		, uint64_t
		, float
		, double
		, std::string
		, vec2
		, vec3
		, vec4
		, quat
		, Handle<GameObject>
		, Guid
		, vector<string>
	>;
}