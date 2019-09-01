#pragma once
#include <gfx/Material.h>
#include <idk_opengl/program/Program.h>

namespace idk::ogl
{
	class OpenGLMaterial
		: public Material
	{
	public:
		RscHandle<ShaderProgram> GetShaderProgram() const;
	private:
		RscHandle<ShaderProgram> program;
	};
}