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
#include <particle/ParticleSystemModules.h>

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

		// projects
        , array<string, 32>

		// graphics
		, RscHandle<Mesh>
		, RscHandle<Material>
		, RscHandle<MaterialInstance>
        , RscHandle<Texture>
		, RscHandle<CubeMap>
		, UniformInstance
		, UniformInstanceValue
		, hash_table<string, UniformInstance>
		, hash_table<string, UniformInstanceValue>
		, CameraClear_t // camera clear
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
		, BasicAnimationState
		, BlendTree
		, std::variant<BasicAnimationState, BlendTree>
		, AnimationState
		, hash_table<string, AnimationState>
		, AnimationLayer
		, hash_table<string, size_t>
		, vector<AnimationLayer>
		, std::array<bool, 100>
		, std::array<string, 2>
		, matrix_decomposition<real>
		, vector<matrix_decomposition<real>>
		, vector<mat4>
		
        // particle system
        , MainModule
        , EmissionModule
        , ShapeModule
        , RendererModule
		
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