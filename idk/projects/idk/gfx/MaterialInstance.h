#pragma once
#include <gfx/Material.h>
#include <gfx/Texture.h>
#include <res/Resource.h>
#include <gfx/UniformInstance.h>

namespace idk
{
    RESOURCE_EXTENSION(MaterialInstance, ".matinst")

	class MaterialInstance
		: public Resource<MaterialInstance>
        , public Saveable<MaterialInstance>
	{
	public:
		RscHandle<Material> material;
		//Change this to be a map of name -> some struct that represents an entire block
		hash_table<string, UniformInstance> uniforms;

		bool IsImageBlock(const string& name)const;
		hash_table<string, RscHandle<Texture>> GetImageBlock(const string& name)const;
		std::optional<UniformInstance> GetUniform(const string& name)const;
		string GetUniformBlock(const string& name)const;
	};
}