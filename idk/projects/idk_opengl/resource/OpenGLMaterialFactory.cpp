#include "pch.h"

#include <sstream>

#include <core/Core.h>
#include <file/FileSystem.h>

#include "OpenGLMaterialFactory.h"

namespace idk::ogl
{
	unique_ptr<Material> OpenGLMaterialFactory::Create()
	{
		return Create(Core::GetSystem<FileSystem>().GetFile("/assets/shader/flat_color.frag"));
	}
	unique_ptr<Material> OpenGLMaterialFactory::Create(FileHandle filepath)
	{
		auto mat = std::make_unique<OpenGLMaterial>();

		return std::move(mat);
	}
}
