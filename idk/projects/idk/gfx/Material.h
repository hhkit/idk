#pragma once
#include <util/enum.h>
#include <res/Resource.h>
#include <res/ResourceMeta.h>

#include <map>
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
		DefaultLit,
		Specular)


	class Material
		: public Resource<Material>
		, public Saveable<Material, false_type>
	{
	public:
		RscHandle<ShaderProgram>            _shader_program  ;
		RscHandle<MaterialInstance>         _default_instance;
        std::map<string, UniformInstance> uniforms         ;
        vector<UniformInstance>             hidden_uniforms  ;

		MaterialDomain domain = MaterialDomain::Surface ;
		BlendMode      blend  = BlendMode::Opaque       ;
		ShadingModel   model  = ShadingModel::DefaultLit;

		RscHandle<ShaderTemplate> GetTemplate() const;
		virtual ~Material();

		EXTENSION(".mat");
	};
}