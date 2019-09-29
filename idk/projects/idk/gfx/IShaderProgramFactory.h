#pragma once
#include <res/ResourceFactory.h>
#include <gfx/ShaderProgram.h>

namespace idk
{
	class IShaderProgramFactory
		: public ResourceFactory<ShaderProgram>
	{
	public:
		virtual ShaderBuildResult BuildGLSL(const RscHandle<ShaderProgram>& program, ShaderStage stage, string_view glsl_code) = 0;
	};
}