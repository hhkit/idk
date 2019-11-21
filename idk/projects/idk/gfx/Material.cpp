#include "stdafx.h"
#include "Material.h"
#include <gfx/ShaderProgram.h>
#include <gfx/GraphicsSystem.h>
namespace idk
{
	RscHandle<ShaderTemplate> Material::GetTemplate() const
	{
		if (domain == MaterialDomain::Surface)
		{
			switch (blend)
			{
			case BlendMode::Opaque:
				switch (model)
				{
				case ShadingModel::DefaultLit: return *Core::GetResourceManager().Load<ShaderTemplate>(Core::GetSystem<GraphicsSystem>().is_deferred()?"/engine_data/shaders/pbr_deferred.tmpt":"/engine_data/shaders/pbr_forward.tmpt");
				case ShadingModel::Unlit:      return *Core::GetResourceManager().Load<ShaderTemplate>(Core::GetSystem<GraphicsSystem>().is_deferred()?"/engine_data/shaders/pbr_deferred.tmpt":"/engine_data/shaders/pbr_forward.tmpt");
				case ShadingModel::Specular:   return *Core::GetResourceManager().Load<ShaderTemplate>(Core::GetSystem<GraphicsSystem>().is_deferred() ? "/engine_data/shaders/pbr_specular.tmpt" : "/engine_data/shaders/pbr_specular.tmpt");
				}
			case BlendMode::Masked:
				switch (model)
				{
				case ShadingModel::DefaultLit: return *Core::GetResourceManager().Load<ShaderTemplate>("/engine_data/shaders/pbr_forward.tmpt");
				case ShadingModel::Unlit:      return *Core::GetResourceManager().Load<ShaderTemplate>("/engine_data/shaders/pbr_forward.tmpt");
				case ShadingModel::Specular:   return *Core::GetResourceManager().Load<ShaderTemplate>("/engine_data/shaders/pbr_specular.tmpt");
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
