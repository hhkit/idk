#include "pch.h"
#include "OpenGLMaterial.h"

namespace idk::ogl
{
	RscHandle<ShaderProgram> OpenGLMaterial::GetShaderProgram() const
	{
		return program;
	}
}
