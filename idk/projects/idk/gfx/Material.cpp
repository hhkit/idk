#include "stdafx.h"
#include "Material.h"

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
				case ShadingModel::DefaultLit: return *Core::GetResourceManager().Load<ShaderTemplate>("/assets/shader/pbr_deferred.tmpt");
				case ShadingModel::Unlit:      return *Core::GetResourceManager().Load<ShaderTemplate>("/assets/shader/flat_color_deferred.tmpt");
				}
			case BlendMode::Masked:
				switch (meta.model)
				{
				case ShadingModel::DefaultLit: return *Core::GetResourceManager().Load<ShaderTemplate>("/assets/shader/pbr_forward.tmpt");
				case ShadingModel::Unlit:      return *Core::GetResourceManager().Load<ShaderTemplate>("/assets/shader/flat_color_forward.tmpt");
				}
			}
		}
		return RscHandle<ShaderTemplate>();
	}
}
