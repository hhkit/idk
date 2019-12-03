#pragma once

#include <anim/Animation.h>
#include <anim/Skeleton.h>
#include <gfx/CompiledMesh.h>

namespace idk
{
	using CompiledVariant = std::variant<
		CompiledMesh
	,	anim::Animation
	,	anim::Skeleton
	>;
}