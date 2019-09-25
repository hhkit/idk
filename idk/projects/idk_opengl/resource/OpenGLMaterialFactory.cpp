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
	unique_ptr<Material> OpenGLMaterialFactory::GenerateDefaultResource()
	{
		auto mat = std::make_unique<OpenGLMaterial>();
		auto meta = mat->GetMeta();
		meta.compiled_shader_guid = Guid{ "10b6e1eb-694f-4509-b636-f22fca6f6b02" };
		mat->SetMeta(meta);
		mat->BuildShader({}, "", default_material_code);
		return mat;
	}
	unique_ptr<Material> OpenGLMaterialFactory::Create()
	{
		auto mat = std::make_unique<OpenGLMaterial>();
		return mat;
	}
}
