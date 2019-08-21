#include "pch.h"
#include "OpenGLMeshFactory.h"
#include <idk_opengl/OpenGLMesh.h>

namespace idk::ogl
{
	unique_ptr<Mesh> OpenGLMeshFactory::Create()
	{
		return std::make_unique<OpenGLMesh>();
	}

	unique_ptr<Mesh> OpenGLMeshFactory::Create(string_view)
	{
		return unique_ptr<Mesh>();
	}

	unique_ptr<Mesh> OpenGLMeshFactory::Create(string_view, const ResourceMeta&)
	{
		return unique_ptr<Mesh>();
	}
}