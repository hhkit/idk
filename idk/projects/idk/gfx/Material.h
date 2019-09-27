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
	};

	class Material
		: public Resource<Material>
		, public MetaTag<MaterialMeta>
		, public Saveable<Material, false_type>
	{
	public:
		RscHandle<ShaderProgram> _shader_program = Guid::Make();
		RscHandle<ShaderTemplate> GetTemplate() const;
		virtual ~Material() = default;
	};
}