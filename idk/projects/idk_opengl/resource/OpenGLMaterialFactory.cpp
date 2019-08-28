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
	unique_ptr<Material> OpenGLMaterialFactory::Create(FileHandle filepath)
	{
		auto mat = std::make_unique<OpenGLMaterial>();
		auto shader_stream = filepath.Open(FS_PERMISSIONS::READ, false);
		std::stringstream stringify;
		stringify << shader_stream.rdbuf();
		mat->Set(stringify.str());

		return std::move(mat);
	}
}
