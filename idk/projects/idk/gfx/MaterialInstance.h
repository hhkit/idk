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
		vector<RscHandle<Texture>> GetImageBlock(const string& name)const;
		std::optional<UniformInstance> GetUniform(const string& name)const;
		bool IsUniformBlock(string_view name)const;
		string GetUniformBlock(const string& name)const;
		using uniform_store_t = decltype(uniforms);
		struct temp_store
		{
			uniform_store_t uniforms;

			bool IsImageBlock(uniform_store_t::iterator itr)const;
			bool IsUniformBlock(uniform_store_t::iterator itr)const;
			vector<RscHandle<Texture>> GetImageBlock(uniform_store_t::iterator itr)const;
			string GetUniformBlock(uniform_store_t::iterator itr)const;
		};

		temp_store get_cache()const;

	};
}