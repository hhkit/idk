#pragma once
#include <util/enum.h>
#include <res/Resource.h>
#include <res/ResourceMeta.h>

namespace idk
{
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


	class Material
		: public Resource<Material>
		, public Saveable<Material, false_type>
	{
	public:
		RscHandle<ShaderProgram>            _shader_program  ;
		RscHandle<MaterialInstance>         _default_instance;
        hash_table<string, UniformInstance> uniforms         ;
        vector<UniformInstance>             hidden_uniforms  ;

		MaterialDomain domain = MaterialDomain::Surface ;
		BlendMode      blend  = BlendMode::Opaque       ;
		ShadingModel   model  = ShadingModel::DefaultLit;

		RscHandle<ShaderTemplate> GetTemplate() const;
		virtual ~Material();

		EXTENSION(".mat");
	};
}