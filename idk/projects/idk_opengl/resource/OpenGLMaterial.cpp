#include "pch.h"
#include "OpenGLMaterial.h"

namespace idk::ogl
{
	void OpenGLMaterial::Set(string_view fragment_code)
	{
	}
	const Program& OpenGLMaterial::GetShaderProgram()
	{
		return program;
	}
}
