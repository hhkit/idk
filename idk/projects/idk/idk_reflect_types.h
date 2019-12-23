#pragma once

#include "idk.h"
#include <core/Handle.h>
#include <res/ResourceHandle.h>
#include <gfx/UniformInstance.h>
#include <gfx/LightTypes.h>
#include <gfx/CameraClear.h>
#include <anim/AnimationState.h>
#include <phys/collidable_shapes.h>
#include <particle/MinMax.h>
#include <math/rect.h>
#include <common/LayerMask.h>

namespace idk::reflect
{
	// add reflected types here... only have to add
	// types that you want the context of in visit.
	using ReflectedTypes = variant<
		bool
		, char
		, unsigned char
        , short
        , uint16_t
		, int
        , uint32_t
		, int64_t
		, uint64_t
		, float
		, double

		, vec2
		, vec3
		, vec4
		, quat
		, mat3
		, mat4
		, color
        , rad
        , rect

        , string
        , Guid

        // handles
		, Handle<GameObject>

		// resource handles
		, RscHandle<Mesh>
		, RscHandle<Material>
		, RscHandle<MaterialInstance>
        , RscHandle<Texture>
		, RscHandle<CubeMap>
        , RscHandle<RenderTarget>
        , RscHandle<ShaderProgram>
        , RscHandle<anim::Skeleton>
        , RscHandle<anim::Animation>
        , RscHandle<Prefab>

        // variants
		, RscHandle<FontAtlas>
		, UniformInstance
		, UniformInstanceValue
		, CameraClear
		, LightVariant
//        , variant<BasicAnimationState, BlendTree>
		, CollidableShapes

        // macro enums
        , MinMaxMode

        // others
        , MinMax<float>
        , MinMax<vec3>
        , MinMax<color>
        , LayerMask
	>;
}