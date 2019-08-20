#pragma once
#include <idk.h>
#include <res/Resource.h>
#include <util/enum.h>

namespace idk
{
	namespace vtx
	{
		ENUM(Attrib, char,
			Position,
			Normal,
			Tangent,
			UV,
			BoneWeight
		);

		enum Flags
		{
			Pos   = 1 << Attrib::Position,
			Norm  = 1 << Attrib::Normal,
			Tan   = 1 << Attrib::Tangent,
			Uv    = 1 << Attrib::UV,
			BoneW = 1 << Attrib::BoneWeight,
		};
	}

	struct attrib_meta
	{
		int         location = 0;
		vtx::Attrib attrib = vtx::Attrib::Position;
	};

	struct renderer_reqs
	{
		vector<attrib_meta> requirements;
	};
}