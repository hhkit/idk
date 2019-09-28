#include "stdafx.h"
#include "Material.h"
#include <gfx/ShaderProgram.h>

namespace idk
{
	RscHandle<ShaderTemplate> Material::GetTemplate() const
	{
		if (meta.domain == MaterialDomain::Surface)
		{
			switch (meta.blend)
			{
			case BlendMode::Opaque:
				switch (meta.model)
				{
				case ShadingModel::DefaultLit: return *Core::GetResourceManager().Load<ShaderTemplate>("/assets/shader/pbr_forward.tmpt");
				case ShadingModel::Unlit:      return *Core::GetResourceManager().Load<ShaderTemplate>("/assets/shader/pbr_forward.tmpt");
				}
			case BlendMode::Masked:
				switch (meta.model)
				{
				case ShadingModel::DefaultLit: return *Core::GetResourceManager().Load<ShaderTemplate>("/assets/shader/pbr_forward.tmpt");
				case ShadingModel::Unlit:      return *Core::GetResourceManager().Load<ShaderTemplate>("/assets/shader/pbr_forward.tmpt");
				}
			}
		}
		return RscHandle<ShaderTemplate>();
	}
	Material::~Material()
	{
		Core::GetResourceManager().Release(_shader_program);
	}
}
