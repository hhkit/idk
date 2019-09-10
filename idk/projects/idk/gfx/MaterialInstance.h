#pragma once
#include <gfx/Material.h>
#include <gfx/Texture.h>
namespace idk
{
	using UniformInstance = variant<bool, float, int, vec2, vec3, vec4, mat3, mat4,RscHandle<Texture>>;

	class MaterialInstance
	{
	public:
		RscHandle<Material> material;
		//Change this to be a map of name -> some struct that represents an entire block
		hash_table<string, UniformInstance> uniforms;
		bool IsImageBlock(const string& name)const
		{
			bool ret = false;
			try
			{
				auto itr = uniforms.find(name);
				if (itr != uniforms.end())
				{
					auto& val =std::get<RscHandle<Texture>>(itr->second);
					val;
					ret = true;
				}
			}
			catch (...)
			{

			}
			return ret;
		}
		hash_table<string,RscHandle<Texture>> GetImageBlock(const string& name)const
		{
			//TODO actually get a block
			hash_table<string,RscHandle<Texture>> result;
			auto itr = uniforms.find(name);
			if (itr != uniforms.end())
			{
				//Todo, replace name with the individual names in the block
				result.emplace(name,std::get<RscHandle<Texture>>(itr->second));
			}
			return result;
		}
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