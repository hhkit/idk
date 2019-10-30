#pragma once

#include "idk.h"
#include <core/Handle.h>
#include <res/ResourceHandle.h>
#include <res/ResourceMeta.h>
#include <particle/MinMax.h>
#include <gfx/UniformInstance.h>
#include <gfx/ShaderGraph_data.h>
#include <gfx/LightTypes.h>
#include <anim/AnimationLayer.h>
#include <phys/collidable_shapes.h>
#include <prefab/PrefabData.h>
#include <prefab/PropertyOverride.h>

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

	struct DontClear {};
	using CameraClear_t =std::variant<DontClear, color, RscHandle<CubeMap>>;
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
        , uint16_t
        , short
        , uint32_t
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

		// graphics
		, RscHandle<Mesh>
		, RscHandle<Material>
		, RscHandle<MaterialInstance>
        , RscHandle<Texture>
		, RscHandle<CubeMap>
        , RscHandle<RenderTarget>
        , RscHandle<ShaderProgram>
		, UniformInstanceValue
		, hash_table<string, UniformInstance>
		, hash_table<string, UniformInstanceValue>
		, CameraClear_t // camera clear
		, LightVariant
        // shadergraph
        , hash_table<Guid, shadergraph::Node>
        , vector<shadergraph::Slot>
        , vector<shadergraph::Link>
        , vector<shadergraph::Parameter>

		// physics
		, CollidableShapes

		// anim
		, RscHandle<anim::Skeleton>
		, RscHandle<anim::Animation>
		, std::variant<BasicAnimationState, BlendTree>
		, hash_table<string, AnimationState>
		, hash_table<string, size_t>
		, vector<AnimationLayer>
		, std::array<bool, 100>
		, std::array<string, 2>
		, vector<mat4>
		
        // particle system
        , MinMax<float>
        , MinMax<vec3>
        , MinMax<color>
		
		// resources
		, vector<SerializedMeta>
		, vector<string>
		, vector<Guid>
		, hash_table<Guid, string>
        , vector<reflect::dynamic>
        , vector<Handle<GameObject>>

		// prefabs
        , vector<PrefabData>
        , vector<PropertyOverride>
        , RscHandle<Prefab>

        // audio
		, vector<RscHandle<AudioClip>>
	>;
}