#include "pch.h"

#include <sstream>

#include <core/Core.h>
#include <file/FileSystem.h>

#include "OpenGLCubemapFactory.h"

namespace idk::ogl
{
	unique_ptr<CubeMap> OpenGLCubemapFactory::GenerateDefaultResource()
	{
		auto mat = std::make_unique<OpenGLCubemap>();
		return mat;
	}
	unique_ptr<CubeMap> OpenGLCubemapFactory::Create()
	{
		auto mat = std::make_unique<OpenGLCubemap>();
		return mat;
	}
	unique_ptr<CubeMap> OpenGLCubemapFactory::Create(PathHandle filepath)
	{
		UNREFERENCED_PARAMETER(filepath);
		auto mat = std::make_unique<OpenGLCubemap>();

		return mat;
	}

	unique_ptr<CubeMap> Create(PathHandle filepath, const CubeMap::Metadata&)
	{
		return unique_ptr<CubeMap>();
	}
}
