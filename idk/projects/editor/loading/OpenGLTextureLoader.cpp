#include "pch.h"
#include <loading/OpenGLTextureLoader.h>
#include <gfx/GraphicsSystem.h>
#include <res/MetaBundle.h>
//Dep
#include <stb/stb_image.h>

//Resources
#include <idk_opengl/resource/OpenGLTexture.h>

namespace idk
{
	ResourceBundle OpenGLTextureLoader::LoadFile(PathHandle path_to_resource)
	{//Assert for now
		assert(Core::GetSystem<GraphicsSystem>().GetAPI() == GraphicsAPI::OpenGL);

		auto texture_handle = Core::GetResourceManager().LoaderEmplaceResource<ogl::OpenGLTexture>();

		auto tm = texture_handle->GetMeta();

		ivec2 size{};
		int channels{};

		auto data = std::unique_ptr<void, decltype(&stbi_image_free)>
		{
			stbi_load(path_to_resource.GetFullPath().data(), &size.x, &size.y, &channels, 0),
			stbi_image_free
		};
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

			texture_handle->Buffer(data.get(), size, col_format);
		}

		return texture_handle;
	}

	ResourceBundle OpenGLTextureLoader::LoadFile(PathHandle path_to_resource, const MetaBundle& path_to_meta)
	{
		auto& metadata = path_to_meta.metadatas[0];
		auto texture_handle = Core::GetResourceManager().LoaderEmplaceResource<ogl::OpenGLTexture>(metadata.guid);

		auto first_meta = metadata.GetMeta<Texture>();
		if (first_meta)
			texture_handle->SetMeta(*first_meta);

		ivec2 size{};
		int channels{};

		auto data = std::unique_ptr<void, decltype(&stbi_image_free)>
		{
			stbi_load(path_to_resource.GetFullPath().data(), &size.x, &size.y, &channels, 0),
			stbi_image_free
		};

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

			texture_handle->Buffer(data.get(), size, col_format);

			return texture_handle;
		}

		return {};
	}

};