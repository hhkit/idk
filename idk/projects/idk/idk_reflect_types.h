#pragma once

#include "idk.h"
#include <core/Handle.h>
#include <res/Guid.h>
#include <res/FileResources.h>

namespace idk
{
	class GameObject;
	class Mesh;
}

namespace idk::reflect
{

	// add reflected types here... only have to add
	// types that are used in other reflected types
	// ie. don't have to add "top-level" reflected types
	using ReflectedTypes = variant<
		int
		, bool
		, char
		, int64_t
		, uint64_t
		, float
		, double
		, string

		, vec2
		, vec3
		, vec4
		, quat
		, mat3
		, mat4
		, color

		, Handle<GameObject>
		, Guid
		, GenericMetadata
		, vector<GenericMetadata>

		, RscHandle<Mesh>
		, variant<bool, float, int, vec2, vec3, vec4, mat3, mat4> // UniformInstance
		, vector<string>
		, hash_table<Guid, string>
	>;
}