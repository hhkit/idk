#pragma once
#include <idk.h>
#include <anim/Animation.h>
#include <anim/Skeleton.h>
#include <gfx/CompiledTexture.h>
#include <gfx/CompiledMesh.h>

namespace idk
{
	using CompiledVariant = 
		variant<
			CompiledTexture
		,	CompiledMesh
		,	CompiledCubeMap
		,	anim::Animation
		,	anim::Skeleton
		>;
}