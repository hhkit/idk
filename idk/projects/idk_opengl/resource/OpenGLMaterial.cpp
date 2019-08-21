#include "pch.h"
#include "OpenGLMaterial.h"

namespace idk::ogl
{
	void OpenGLMaterial::Set(string_view fragment_code)
	{
		program.Attach(Shader{
			GL_FRAGMENT_SHADER,
			fragment_code
			});
		program.Link();
	}

	const Program& OpenGLMaterial::GetShaderProgram() const
	{
		return program;
	}
}
