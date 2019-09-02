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

	struct MaterialMeta
	{
		MaterialDomain domain = MaterialDomain::Surface;
		BlendMode      blend  = BlendMode::Opaque;
		ShadingModel   model  = ShadingModel::Unlit;
		Guid           compiled_shader_guid = Guid::Make();
	};

	class Material
		: public Resource<Material>
		, public MetaTag<MaterialMeta>
	{
	public:
		virtual ~Material() = default;

		virtual bool BuildShader(RscHandle<ShaderTemplate>, string_view material_uniforms, string_view material_code) = 0;
		RscHandle<ShaderProgram> GetShaderProgram() const;
	protected:
		RscHandle<ShaderProgram> _program{meta.compiled_shader_guid};
		void OnMetaUpdate(const MaterialMeta& newmeta) override;
	};
}