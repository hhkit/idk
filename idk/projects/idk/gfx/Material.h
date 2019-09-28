#pragma once
#include <util/enum.h>
#include <res/Resource.h>
#include <res/ResourceMeta.h>

namespace idk
{
	RESOURCE_EXTENSION(Material, ".mat");

	class ShaderTemplate;

	ENUM(MaterialDomain, char,
		Surface,
		PostProcess)

	ENUM(BlendMode, char,
		Opaque,
		Masked)

	ENUM(ShadingModel, char,
		Unlit,
		DefaultLit)

	struct MaterialMeta
	{
		MaterialDomain domain = MaterialDomain::Surface;
		BlendMode      blend  = BlendMode::Opaque;
		ShadingModel   model  = ShadingModel::DefaultLit;
	};

	class Material
		: public Resource<Material>
		, public MetaTag<MaterialMeta>
		, public Saveable<Material, false_type>
	{
	public:
		RscHandle<ShaderProgram> _shader_program;

        //Change this to be a map of name -> some struct that represents an entire block
        hash_table<string, UniformInstance> uniforms;

		RscHandle<ShaderTemplate> GetTemplate() const;
		virtual ~Material();
	};
}