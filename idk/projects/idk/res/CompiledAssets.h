#pragma once
#include <idk.h>
#include <anim/Animation.h>
#include <anim/Skeleton.h>
#include <gfx/CompiledMesh.h>
#include <gfx/CompiledTexture.h>
#include <prefab/Prefab.h>

namespace idk
{
	using CompiledVariant = variant<
		CompiledMesh
	,	CompiledTexture
	,	anim::Animation
	,	anim::Skeleton
	,	Prefab
	>;
}