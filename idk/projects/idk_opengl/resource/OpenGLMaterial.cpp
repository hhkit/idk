#include "pch.h"
#include "OpenGLMaterial.h"
#include <core/Core.h>
#include <idk_opengl/program/Program.h>
#include <gfx/ShaderTemplate.h>
#include <file/FileSystem.h>

namespace idk::ogl
{
	bool OpenGLMaterial::BuildShader(RscHandle<ShaderTemplate> lighting_model, string_view material_uniforms, string_view material_code)
	{
		//auto path = "/assets/shader/" + string{ meta.compiled_shader_guid } +".gen.frag";
		//Core::GetSystem<FileSystem>().Open(path, FS_PERMISSIONS::WRITE) << lighting_model->Instantiate(material_uniforms, material_code);
		//Core::GetResourceManager().LoadFile(path.data(), MetaFile{ {meta.compiled_shader_guid} });
		Core::GetResourceManager().Free(RscHandle<ShaderProgram>{meta.compiled_shader_guid});
		auto prog = Core::GetResourceManager().LoaderEmplaceResource<Program>(meta.compiled_shader_guid);
		prog->Attach(
			Shader{
				GL_FRAGMENT_SHADER,
				lighting_model->Instantiate(material_uniforms, material_code)
			}
		).Link();

		return false;
	}
}
