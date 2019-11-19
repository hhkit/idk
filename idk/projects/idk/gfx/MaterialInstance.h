#pragma once
#include <gfx/Material.h>
#include <gfx/Texture.h>
#include <res/Resource.h>
#include <gfx/UniformInstance.h>

namespace idk
{
	class MaterialInstance
		: public Resource<MaterialInstance>
        , public Saveable<MaterialInstance>
	{
	public:
		using UniformStorage = hash_table<string, UniformInstanceValue>;
		struct temp_store
		{
			UniformStorage uniforms;

			bool IsImageBlock(UniformStorage::iterator itr) const;
			bool IsUniformBlock(UniformStorage::iterator itr) const;
			vector<RscHandle<Texture>> GetImageBlock(UniformStorage::iterator itr) const;
			string GetUniformBlock(UniformStorage::iterator itr) const;
		};

		RscHandle<Material> material;
		UniformStorage uniforms;

		bool IsImageBlock(const string& name) const;
		vector<RscHandle<Texture>> GetImageBlock(const string& name) const;
		std::optional<UniformInstanceValue> GetUniform(const string& name) const;
		void SetUniform(const string& name, UniformInstanceValue value);

		bool IsUniformBlock(string_view name) const;
		string GetUniformBlock(const string& name) const;

		temp_store get_cache()const;

		EXTENSION(".matinst");
	};
}