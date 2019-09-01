#pragma once
#include <gfx/Material.h>
#include <idk_opengl/program/Program.h>
namespace idk::ogl
{
	class FragmentTemplate;

	class OpenGLMaterial
		: public Material
	{
	public:
		bool BuildShader(RscHandle<ShaderTemplate> lighting_model, string_view material_uniforms, string_view material_code) override;
	};
}