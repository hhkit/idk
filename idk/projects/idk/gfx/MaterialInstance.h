#pragma once
#include <gfx/Material.h>

namespace idk
{
	using UniformInstance = std::variant<bool, float, int, vec2, vec3, vec4, mat3, mat4>;

	class MaterialInstance
	{
	public:
		RscHandle<Material> material;
		hash_table<string, UniformInstance> uniforms;
	};
}