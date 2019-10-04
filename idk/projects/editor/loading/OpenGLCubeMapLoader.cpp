#include "pch.h"
#include <loading/OpenGLCubeMapLoader.h>
#include <gfx/GraphicsSystem.h>
#include <res/MetaBundle.h>
#include <filesystem>

//Dep
#include <stb/stb_image.h>

//Resources
#include <gfx/CubeMap.h>
#include <idk_opengl/system/OpenGLGraphicsSystem.h>
#include <idk_opengl/resource/OpenGLCubemap.h>

namespace idk
{
	ResourceBundle OpenGLCubeMapLoader::LoadFile(PathHandle path_to_resource)
	{//Assert for now
		assert(Core::GetSystem<GraphicsSystem>().GetAPI() == GraphicsAPI::OpenGL);

		const auto texture_handle = Core::GetResourceManager().LoaderEmplaceResource<ogl::OpenGLCubemap>();

		auto tm = texture_handle->GetMeta();
		//auto texture_id = texture_handle->ID();

		ivec2 size{};
		int channels{};

		static string fileExt[] = {
			".r",
			".l",
			".t",
			".d",
			".b",
			".f"
		};

		std::filesystem::path path{ path_to_resource.GetMountPath() };

		auto cubemap = path.stem();
		auto ext = cubemap.extension();

		texture_handle->Bind();
		for (int i = 0; i < std::size(fileExt); ++i)
		{
			auto pp = (path.parent_path()).string() + "/" + (cubemap.stem()).string();
			auto p = pp + fileExt[i] + ext.string();

			auto data = stbi_load(PathHandle{ p }.GetFullPath().data(), &size.x, &size.y, &channels, 0);

			//assert(data);

			const auto col_format = [&]() -> CMInputChannels
			{	switch (channels)
			{
			default:
			case 1: return CMInputChannels::RED;
			case 2: return CMInputChannels::RG;
			case 3: return CMInputChannels::RGB;
			case 4: return CMInputChannels::RGBA;
			}
			}();

			texture_handle->Buffer(i, data, size, col_format);
			stbi_image_free(data);
		}
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
		Core::GetSystem<ogl::Win32GraphicsSystem>().EnqueueCubemapForConvolution(texture_handle);
		return texture_handle;
	}

	ResourceBundle OpenGLCubeMapLoader::LoadFile(PathHandle path_to_resource, const MetaBundle& path_to_meta)
	{
		//assert(false);
		assert(Core::GetSystem<GraphicsSystem>().GetAPI() == GraphicsAPI::OpenGL);
		auto& metadata = path_to_meta.metadatas[0];

		const auto cubemap_handle = Core::GetResourceManager().LoaderEmplaceResource<ogl::OpenGLCubemap>(metadata.guid);

		auto tm = cubemap_handle->GetMeta();
		//auto texture_id = texture_handle->ID();


		auto first_meta = metadata.GetMeta<CubeMap>();
		if (first_meta)
			cubemap_handle->SetMeta(*first_meta);

		ivec2 size{};
		int channels{};

		static string fileExt[] = {
			".r",
			".l",
			".t",
			".d",
			".b",
			".f"
		};

		std::filesystem::path path{ path_to_resource.GetMountPath() };

		auto cubemap = path.stem();
		auto ext = cubemap.extension();

		cubemap_handle->Bind();
		for (int i = 0; i < std::size(fileExt); ++i)
		{
			auto pp = (path.parent_path()).string() + "/" + (cubemap.stem()).string();
			auto p = pp + fileExt[i] + ext.string();

			auto data = stbi_load(PathHandle{ p }.GetFullPath().data(), &size.x, &size.y, &channels, 0);

			//assert(data);

			const auto col_format = [&]() -> CMInputChannels
			{	switch (channels)
			{
			default:
			case 1: return CMInputChannels::RED;
			case 2: return CMInputChannels::RG;
			case 3: return CMInputChannels::RGB;
			case 4: return CMInputChannels::RGBA;
			}
			}();

			cubemap_handle->Buffer(i, data, size, col_format);
			stbi_image_free(data);
		}
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
		Core::GetSystem<ogl::Win32GraphicsSystem>().EnqueueCubemapForConvolution(cubemap_handle);

		return cubemap_handle;
	}

};