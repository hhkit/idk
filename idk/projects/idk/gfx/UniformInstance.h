#pragma once
#include <idk.h>
namespace idk
{
	using UniformInstance = variant<bool, float, int, vec2, vec3, vec4, mat3, mat4, RscHandle<Texture>>;
}