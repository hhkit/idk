#include "pch.h"
#include "ProgramFactory.h"
#include <opengl/program/Program.h>
#include <util/ioutils.h>
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

	unique_ptr<ShaderProgram> ProgramFactory::Create(PathHandle h)
	{
		LOG_TO(LogPool::GFX, "loading %s", h.GetMountPath().data());
		auto shader_stream = h.Open(FS_PERMISSIONS::READ);
		auto shader_code = stringify(shader_stream);
		auto program = std::make_unique<ogl::Program>();

		const auto shader_enum = [](std::string_view ext)->GLenum
		{
			switch (string_hash(ext))
			{
			case string_hash(".vert"): return GL_VERTEX_SHADER;
			case string_hash(".geom"): return GL_GEOMETRY_SHADER;
			case string_hash(".tese"): return GL_TESS_EVALUATION_SHADER;
			case string_hash(".tesc"): return GL_TESS_CONTROL_SHADER;
			case string_hash(".frag"): return GL_FRAGMENT_SHADER;
			case string_hash(".comp"): return GL_COMPUTE_SHADER;
			default:                   return 0;
			}
		}(h.GetExtension());
		program->Attach(Shader{ shader_enum, shader_code.data() });
		program->Link();
		return program;
	}
}
