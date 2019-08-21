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
			Tangent,
			UV,
			BoneID,
			BoneWeight
		)

		enum Flags : int
		{
			Pos    = 1 << Attrib::Position,
			Norm   = 1 << Attrib::Normal,
			Tan    = 1 << Attrib::Tangent,
			Uv     = 1 << Attrib::UV,
			BoneId = 1 << Attrib::BoneID,
			BoneWt  = 1 << Attrib::BoneWeight,
		};

		using NativeType = std::tuple<
			vec3,  // Position
			vec3,  // Normal
			vec3,  // Tangent
			vec2,  // UV
			ivec4, // ID
			vec4   // Weight
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
	struct renderer_reqs
	{
		using Location = int;
		hash_table<vtx::Attrib, Location> requirements;
	};
}