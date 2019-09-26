#pragma once
#include <idk.h>
#include <gfx/CubeMap.h>
namespace idk
{
	vec2 spherical_projection(vec3 v);
	std::pair<vec2, TextureTarget> cube_projection(vec3 vec);
}