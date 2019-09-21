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

		ivec2 size{};
		int channels{};

		auto data = stbi_load(path_to_resource.GetFullPath().data(), &size.x, &size.y, &channels, 0);

		if (data) // stbi image can fail
		{
			auto col_format = [&]() -> InputChannels
			{	switch (channels)
			{
			default:
			case 1: return InputChannels::RED;
			case 2: return InputChannels::RG;
			case 3: return InputChannels::RGB;
			case 4: return InputChannels::RGBA;
			}
			}();

			texture_handle.as<ogl::OpenGLTexture>().Buffer(data, size, col_format);

			retval.resources.emplace_back(texture_handle);
		}
		return retval;
	}

	FileResources OpenGLTextureLoader::Create(PathHandle path_to_resource, const MetaFile& path_to_meta)
	{//Assert for now
		assert(Core::GetSystem<GraphicsSystem>().GetAPI() == GraphicsAPI::OpenGL);

		FileResources retval;

		auto texture_handle = Core::GetResourceManager().Emplace<ogl::OpenGLTexture>(path_to_meta.guids[0]);

		auto& first_meta = path_to_meta.resource_metas[0];
		if (first_meta.is<Texture::Metadata>())
			texture_handle->SetMeta(first_meta.get<Texture::Metadata>());

		ivec2 size{};
		int channels{};

		auto data = stbi_load(path_to_resource.GetFullPath().data(), &size.x, &size.y, &channels, 0);

		if (data) // stbi image can fail
		{
			auto col_format = [&]() -> InputChannels
			{	switch (channels)
			{
			default:
			case 1: return InputChannels::RED;
			case 2: return InputChannels::RG;
			case 3: return InputChannels::RGB;
			case 4: return InputChannels::RGBA;
			}
			}();

			texture_handle.as<ogl::OpenGLTexture>().Buffer(data, size, col_format);

			retval.resources.emplace_back(RscHandle<Texture>{texture_handle});
		}
		return retval;
	}

};