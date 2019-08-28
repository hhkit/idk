#include "pch.h"
#include "OpenGLFBXLoader.h"
#include <assimp/Importer.hpp>
#include <gfx/GraphicsSystem.h>
#include <idk_opengl/system/OpenGLGraphicsSystem.h>
#include <idk_opengl/resource/OpenGLMesh.h>

namespace idk
{
	FileResources OpenGLFBXLoader::Create(FileHandle path_to_resource)
	{
		switch (Core::GetSystem<GraphicsSystem>().GetAPI())
		{
		case GraphicsAPI::OpenGL:
		{
			auto mesh_handle = Core::GetResourceManager().Create<Mesh>();
			auto& opengl_mesh = mesh_handle.as<ogl::OpenGLMesh>();

		// see idk::ogl::OpenGLMeshFactory for how to create meshes
		//	opengl_mesh.Bind();
		}
		case GraphicsAPI::Vulkan:

		default:
			break;
		}
		return FileResources();
	}
	FileResources OpenGLFBXLoader::Create(FileHandle path_to_resource, span<SerializedResourceMeta> path_to_meta)
	{
		return FileResources();
	}
}