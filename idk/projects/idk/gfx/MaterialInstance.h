#pragma once
#include <gfx/Material.h>
#include <gfx/Texture.h>
#include <res/Resource.h>
#include <res/ResourceExt.h>
#include <gfx/UniformInstance.h>

namespace idk
{
	class MaterialInstance
		: public Resource<MaterialInstance>
        , public Saveable<MaterialInstance>
	{
	public:
		RscHandle<Material> material;
		std::map<string, UniformInstanceValue> uniforms;

		EXTENSION(".matinst");

		bool IsImageBlock(const string& name)const;
		vector<RscHandle<Texture>> GetImageBlock(const string& name)const;
		std::optional<UniformInstanceValue> GetUniform(const string& name)const;
		void SetUniform(const string& name, UniformInstanceValue value);
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