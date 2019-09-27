#include "pch.h"

#include <sstream>

#include <core/Core.h>
#include <file/FileSystem.h>

#include "OpenGLCubeMapFactory.h"

static const auto default_material_code = R"(
	color = vec4(1.0);
)";

namespace idk::ogl
{
	unique_ptr<CubeMap> OpenGLCubeMapFactory::GenerateDefaultResource()
	{
		auto mat = std::make_unique<CubeMap>();
		
		/*
		auto meta = mat->GetMeta();
		meta.compiled_shader_guid = Guid{ "10b6e1eb-694f-4509-b636-f22fca6f6b02" };
		mat->SetMeta(meta);
		mat->BuildShader({}, "", default_material_code);
		return mat;
		*/

		mat->GetMeta();

		return mat;
	}
	unique_ptr<CubeMap> OpenGLCubeMapFactory::Create()
	{
		auto mat = std::make_unique<CubeMap>();
		return mat;
	}
	//unique_ptr<CubeMap> OpenGLCubeMapFactory::Create(PathHandle filepath)
	//{
	//	UNREFERENCED_PARAMETER(filepath);
	//	return Create();
	//}
}
