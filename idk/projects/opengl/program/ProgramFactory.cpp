#include "pch.h"
#include "ProgramFactory.h"
#include <opengl/program/Program.h>
namespace idk::ogl
{
	ShaderBuildResult ProgramFactory::BuildGLSL(const RscHandle<ShaderProgram>& program, ShaderStage stage, string_view glsl_code)
	{
		constexpr static auto arr = std::array<GLenum, 6>{
			GL_VERTEX_SHADER          ,
			GL_TESS_CONTROL_SHADER    ,
			GL_TESS_EVALUATION_SHADER ,
			GL_GEOMETRY_SHADER        ,
			GL_FRAGMENT_SHADER        ,
			GL_COMPUTE_SHADER         ,
		};
		
		RscHandle<ogl::Program>{ program }
			->Attach(Shader{ arr[static_cast<int>(stage)], glsl_code })
			.Link();

		return ShaderBuildResult::Ok;
	}
	unique_ptr<ShaderProgram> ProgramFactory::GenerateDefaultResource()
	{
		return std::make_unique<ogl::Program>();
	}

	unique_ptr<ShaderProgram> ProgramFactory::Create()
	{
		return std::make_unique<ogl::Program>();
	}
}
