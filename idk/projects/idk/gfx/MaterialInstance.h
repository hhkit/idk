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
		hash_table<string, UniformInstanceValue> uniforms;

		bool IsImageBlock(const string& name)const;
		vector<RscHandle<Texture>> GetImageBlock(const string& name)const;
		std::optional<UniformInstanceValue> GetUniform(const string& name)const;
		bool IsUniformBlock(string_view name)const;
		string GetUniformBlock(const string& name)const;
	};
}