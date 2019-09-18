#include "pch.h"
#include <loading/OpenGLTextureLoader.h>
#include <gfx/GraphicsSystem.h>

//Dep
#include <stb/stb_image.h>

//Resources
#include <idk_opengl/resource/OpenGLTexture.h>

namespace idk
{
	FileResources OpenGLTextureLoader::Create(PathHandle path_to_resource)
	{

		//Assert for now
		assert(Core::GetSystem<GraphicsSystem>().GetAPI() == GraphicsAPI::OpenGL);
		
		FileResources retval;

		auto texture_handle = Core::GetResourceManager().Create<Texture>();

		auto tm = texture_handle->GetMeta();
		//auto texture_id = (unsigned int)texture_handle->ID();

		ivec2 size{};
		int channels{};

		auto data = stbi_load(path_to_resource.GetFullPath().data(), &size.x, &size.y, &channels, 0);

		assert(data);

		texture_handle.as<ogl::OpenGLTexture>().Buffer(data, size, tm.internal_format);

		//auto& tHandle = RscHandle<Texture>(texture_handle.guid);

		//auto thandl = RscHandle{ texture_handle }.as<Texture>();

		retval.resources.emplace_back(texture_handle);

		return retval;
	}

	FileResources OpenGLTextureLoader::Create(PathHandle path_to_resource, const MetaFile& path_to_meta)
	{
		UNREFERENCED_PARAMETER(path_to_resource);
		UNREFERENCED_PARAMETER(path_to_meta);

		return Create(path_to_resource);
	}

};