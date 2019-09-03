#pragma once
#include <gfx/Material.h>
namespace idk::vkn
{
	class VulkanMaterial
		: public Material
	{
	public:
		bool BuildShader(RscHandle<ShaderTemplate> lighting_model, string_view material_uniforms, string_view material_code) override;
	};
}