#pragma once
#include <gfx/Material.h>
#include <idk_opengl/program/Program.h>

namespace idk::ogl
{
	class OpenGLMaterial
		: public Material
	{
	public:
		void Set(string_view fragment_code);
		void Set(const ShaderGraph&) {};

		const Program& GetShaderProgram();
	private:
		Program program;
	};
}