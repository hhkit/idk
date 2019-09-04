#pragma once
#include <gfx/Material.h>

namespace idk
{
	using UniformInstance = variant<bool, float, int, vec2, vec3, vec4, mat3, mat4>;

	class MaterialInstance
	{
	public:
		RscHandle<Material> material;
		//Change this to be a map of name -> some struct that represents an entire block
		hash_table<string, UniformInstance> uniforms;
		string GetUniformBlock(const string& name)const
		{
			auto& uniform_block = uniforms;
			string result;
			auto itr = uniform_block .find(name);
			if (itr != uniform_block .end())
			{
				//Replace this with something that grabs all the data in the block and formats it according to the interface block rules.(Padding etc)
				result = std::visit([](auto& value) { return string{ r_cast<const char*>(&value),sizeof(value)}; }, itr->second);
			}
			return result;
		}
	};
}