#pragma once
#include <gfx/Material.h>
namespace idk::vkn
{
	class VulkanMaterial
		: public Material
	{
	public:
		bool BuildShader(RscHandle<ShaderTemplate> lighting_model, string_view material_uniforms, string_view material_code,string_view material_id);
		bool BuildShader(RscHandle<ShaderTemplate> lighting_model, string_view material_uniforms, string_view material_code)override
		{
			return BuildShader(lighting_model, material_uniforms, material_code, "unknown_material");
		}
	};
}