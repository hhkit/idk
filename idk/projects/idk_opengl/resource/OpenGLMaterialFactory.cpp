#include "pch.h"

#include <sstream>

#include <core/Core.h>
#include <file/FileSystem.h>

#include "OpenGLMaterialFactory.h"

namespace idk::ogl
{
	unique_ptr<Material> OpenGLMaterialFactory::Create()
	{
		return unique_ptr<Material>();
	}
	unique_ptr<Material> OpenGLMaterialFactory::Create(string_view filepath)
	{
		auto mat = std::make_unique<OpenGLMaterial>();
		auto shader_stream = Core::GetSystem<FileSystem>().Open(filepath, FS_PERMISSIONS::READ, false);
		std::stringstream stringify;
		stringify << shader_stream.rdbuf();
		mat->Set(stringify.str());

		return std::move(mat);
	}
	unique_ptr<Material> OpenGLMaterialFactory::Create(string_view, const ResourceMeta&)
	{
		return unique_ptr<Material>();
	}
}
