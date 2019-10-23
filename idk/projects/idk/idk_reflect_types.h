#pragma once

#include "idk.h"
#include <core/Handle.h>
#include <res/Guid.h>
#include <prefab/PrefabData.h>
#include <prefab/PropertyOverride.h>
#include <gfx/UniformInstance.h>
#include <gfx/ShaderGraph_data.h>
#include <gfx/LightTypes.h>
#include <math/shapes.h>
#include <res/ResourceMeta.h>
#include <scene/SceneManager.h>
#include <phys/collidable_shapes.h>
#include <math/matrix_decomposition.h>
#include <anim/AnimationLayer.h>
namespace idk
{
	class GameObject;
	class Mesh;
    class Prefab;
	namespace anim
	{
		class Animation;
		class Skeleton;
	}
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
        , rad

		// game objects
		, Handle<GameObject>
		, Guid

		// projects
		, vector<SceneManager::SceneBlock>
        , array<string, 32>

		// graphics
		, RscHandle<Mesh>
		, RscHandle<Material>
		, RscHandle<MaterialInstance>
        , RscHandle<Texture>
		, UniformInstance
		, UniformInstanceValue
		, hash_table<string, UniformInstance>
		, hash_table<string, UniformInstanceValue>
		, std::variant<color, RscHandle<CubeMap>> // camera clear
		, PointLight
		, DirectionalLight
		, SpotLight
		, LightVariant

		// physics
		, box
		, sphere
		, capsule
		, CollidableShapes

		// anim
		, RscHandle<anim::Skeleton>
		, RscHandle<anim::Animation>
		, AnimationState
		, hash_table<string, AnimationState>
		, AnimationLayer
		, hash_table<string, size_t>
		, vector<AnimationLayer>
		, std::array<bool, 100>
		, matrix_decomposition<real>
		, vector<matrix_decomposition<real>>
		, vector<mat4>
		
		
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

		, RscHandle<RenderTarget>
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