#pragma once
#include <gfx/IShaderProgramFactory.h>

namespace idk::ogl
{
	class ProgramFactory
		: public IShaderProgramFactory
	{
	public:
		ShaderBuildResult BuildGLSL(const RscHandle<ShaderProgram>& program, ShaderStage stage, string_view glsl_code);

		unique_ptr<ShaderProgram> GenerateDefaultResource() override;
		unique_ptr<ShaderProgram> Create()                  override;
		unique_ptr<ShaderProgram> Create(PathHandle h)      override;
	};
}