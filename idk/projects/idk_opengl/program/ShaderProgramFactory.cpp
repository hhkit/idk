#include "pch.h"
#include "ShaderProgramFactory.h"
#include <idk_opengl/program/Program.h>
#include <sstream>
namespace idk::ogl
{
	unique_ptr<ShaderProgram> ShaderProgramFactory::GenerateDefaultResource()
	{
		return std::make_unique<ogl::Program>();
	}

	unique_ptr<ShaderProgram> ShaderProgramFactory::Create(FileHandle filepath)
	{
		auto program = std::make_unique<ogl::Program>();
		auto shader_stream = filepath.Open(FS_PERMISSIONS::READ, false);

		std::stringstream stringify;
		stringify << shader_stream.rdbuf();

		auto shader_enum = [](std::string_view ext)->GLenum
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
		}(filepath.GetExtension());

		program->Attach(Shader{ shader_enum, stringify.str() });
		program->Link();

		return std::move(program);
	}
}