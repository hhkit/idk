#include "pch.h"

#include <sstream>

#include <core/Core.h>
#include <file/FileSystem.h>

#include "OpenGLMaterialFactory.h"

static const auto default_material_code = R"(
	color = vec4(1.0);
)";

namespace idk::ogl
{
	unique_ptr<Material> OpenGLMaterialFactory::Create()
	{
		auto mat = std::make_unique<OpenGLMaterial>();
		mat->BuildShader({}, "", default_material_code);
		return mat;
	}
	unique_ptr<Material> OpenGLMaterialFactory::Create(FileHandle filepath)
	{
		UNREFERENCED_PARAMETER(filepath);
		return Create();
	}
}
