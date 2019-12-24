#include "pch.h"
#include "GLSLLoader.h"
#include <core/Core.h>
#include <res/MetaBundle.h>
#include <res/ResourceManager.inl>
#include <opengl/program/Program.h>
#include <sstream>
namespace idk::ogl
{
	ResourceBundle GLSLLoader::LoadFile(PathHandle filepath, const MetaBundle& bundle)
	{
		LOG_TO(LogPool::GFX, "loading %s", filepath.GetMountPath().data());
		const auto* meta = bundle.FetchMeta<Program>();
		const auto program = meta ? Core::GetResourceManager().LoaderEmplaceResource<ogl::Program>(meta->guid)
			: Core::GetResourceManager().LoaderEmplaceResource<ogl::Program>();
		auto shader_stream = filepath.Open(FS_PERMISSIONS::READ, false);

		std::stringstream stringify;
		stringify << shader_stream.rdbuf();

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
		}(filepath.GetExtension());
		program->Attach(Shader{ shader_enum, stringify.str() });
		program->Link();

		return program;
	}
}