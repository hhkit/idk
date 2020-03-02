#include "stdafx.h"
#include "Material.h"
#include <gfx/ShaderProgram.h>
#include <gfx/GraphicsSystem.h>
#include <res/ResourceManager.inl>
#include <res/ResourceHandle.inl>

#include <ds/result.inl>
namespace idk
{
	RscHandle<ShaderTemplate> Material::GetTemplate() const
	{
		if (domain == MaterialDomain::Surface)
		{
#define LOAD(path) *Core::GetResourceManager().Load<ShaderTemplate>(path);
			switch (blend)
			{
			case BlendMode::Opaque:
				switch (model)
				{
				case ShadingModel::DefaultLit: return LOAD(Core::GetSystem<GraphicsSystem>().is_deferred()?"/engine_data/shaders/pbr_deferred.tmpt":"/engine_data/shaders/pbr_forward.tmpt");
				case ShadingModel::Unlit:      return LOAD(Core::GetSystem<GraphicsSystem>().is_deferred()?"/engine_data/shaders/unlit.tmpt":"/engine_data/shaders/pbr_forward.tmpt");
				case ShadingModel::Specular:   return LOAD(Core::GetSystem<GraphicsSystem>().is_deferred() ? "/engine_data/shaders/pbr_deferred_specular.tmpt" : "/engine_data/shaders/pbr_specular.tmpt");
				}
			case BlendMode::Masked:
				switch (model)
				{
				case ShadingModel::DefaultLit: return LOAD("/engine_data/shaders/pbr_forward.tmpt");
				case ShadingModel::Unlit:      return LOAD(Core::GetSystem<GraphicsSystem>().is_deferred() ? "/engine_data/shaders/unlit.tmpt" : "/engine_data/shaders/pbr_forward.tmpt");
				case ShadingModel::Specular:   return LOAD("/engine_data/shaders/pbr_specular.tmpt");
				}
			}
#undef LOAD
		}
		return RscHandle<ShaderTemplate>();
	}
	Material::~Material()
	{
		Core::GetResourceManager().Release(_shader_program);
	}
}
