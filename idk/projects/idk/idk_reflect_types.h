#pragma once

#include "idk.h"
#include <core/Handle.h>
#include <res/Guid.h>
#include <prefab/PrefabData.h>
#include <prefab/PropertyOverride.h>
#include <gfx/UniformInstance.h>
#include <gfx/ShaderGraph_data.h>
#include <res/ResourceMeta.h>
#include <scene/SceneManager.h>
#include <phys/Collider.h>

namespace idk
{
	class GameObject;
	class Mesh;
    class Prefab;
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
		, unsigned char
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

		// game objects
		, Handle<GameObject>
		, Guid

		// projects
		, vector<SceneManager::SceneBlock>

		// graphics
		, RscHandle<Mesh>
		, UniformInstance
		, hash_table<string, UniformInstance>
		//, LightVariant

		// physics
		, Collider::Shapes

		// resources
		, vector<SerializedMeta>

		, vector<string>
		, vector<Guid>
		, hash_table<Guid, string>
        , vector<reflect::dynamic>
        , vector<Handle<GameObject>>

		// prefabs
        , PrefabData
        , vector<PrefabData>
        , PropertyOverride
        , vector<PropertyOverride>
        , RscHandle<Prefab>

		, RscHandle<ShaderProgram>
        , shadergraph::ValueType
        , shadergraph::Slot
        , shadergraph::Node
        , shadergraph::Link
        , shadergraph::Parameter
        , hash_table<Guid, shadergraph::Node>
        , vector<shadergraph::Slot>
        , vector<shadergraph::Link>
        , vector<shadergraph::Parameter>
	>;
}