#pragma once
#include <util/enum.h>
#include <res/Resource.h>
#include <res/ResourceMeta.h>

namespace idk
{
	class ShaderGraph;

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
	{
	public:
		//virtual void Set(string_view fragment_code) = 0;
		//virtual void Set(const ShaderGraph&) = 0;
		virtual ~Material() = default;
	};
}