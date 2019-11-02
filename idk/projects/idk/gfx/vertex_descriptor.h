#pragma once
#include <idk.h>
#include <res/Resource.h>
#include <util/enum.h>

namespace idk
{
	namespace vtx
	{
		ENUM(Attrib, int,
			Position,
			Normal,
			UV,
			Tangent,
			Bitangent,
			BoneID,
			BoneWeight,
            Color,
            ParticlePosition,
            ParticleRotation,
            ParticleSize,
			ModelViewTransform,
			ModelViewNormalTransform
		)

		using NativeType = std::tuple<
			vec3  // Position
		,	vec3  // Normal
		,	vec2  // UV
		,	vec3  // Tangent
		,	vec3  // Bitangent
		,	ivec4 // ID
		,	vec4  // Weight
		,	color // Color
		,	vec3  // particle pos
		,	float // particle rot
        ,   float // particle size
		,   mat4  // modelview matrix
		,	mat4  // modelview normal matrix
		>;
	}
}

namespace std
{
	template<>
	struct hash<idk::vtx::Attrib>
	{
		size_t operator()(const idk::vtx::Attrib& attrib) const noexcept
		{
			return std::hash<idk::vtx::Attrib::UnderlyingType>{}(static_cast<idk::vtx::Attrib::UnderlyingType>(attrib));
		}
	};
}

namespace idk
{
	struct renderer_attributes
	{
		using Location = int;
		hash_table<vtx::Attrib, Location> mesh_attributes;
		hash_table<vtx::Attrib, Location> instanced_attributes;
	};
}