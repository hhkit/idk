#include "pch.h"
#include <opengl/resource/OpenGLCubeMapLoader.h>
#include <gfx/GraphicsSystem.h>
#include <res/MetaBundle.h>
#include <res/ResourceManager.inl>
#include <res/ResourceHandle.inl>
#include <filesystem>

//Dep
#include <stb/stb_image.h>

//Resources
#include <gfx/CubeMap.h>
#include <opengl/system/OpenGLGraphicsSystem.h>
#include <opengl/resource/OpenGLCubemap.h>
#include <res/Guid.inl>

namespace idk
{
	ResourceBundle OpenGLCubeMapLoader::LoadFile(PathHandle path_to_resource, const MetaBundle& metabundle)
	{
		//assert(false);
		assert(Core::GetSystem<GraphicsSystem>().GetAPI() == GraphicsAPI::OpenGL);
		auto meta = metabundle.FetchMeta<CubeMap>();

		const auto cubemap_handle = meta 
			? Core::GetResourceManager().LoaderEmplaceResource<ogl::OpenGLCubemap>(meta->guid)
			: Core::GetResourceManager().LoaderEmplaceResource<ogl::OpenGLCubemap>();

		auto tm = cubemap_handle->GetMeta();
		//auto texture_id = texture_handle->ID();

		if (meta)
			cubemap_handle->SetMeta(*meta->GetMeta<CubeMap>());

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
			string pp = (path.parent_path()).string() + "/" + (cubemap.stem()).string();
			auto p = pp + fileExt[i] + string(ext.string());

			auto data = stbi_load(PathHandle{ p }.GetFullPath().data(), &size.x, &size.y, &channels, 0);

			//assert(data);

			const auto col_format = [&]() -> InputChannels
			{	switch (channels)
			{
			default:
			case 1: return InputChannels::RED;
			case 2: return InputChannels::RG;
			case 3: return InputChannels::RGB;
			case 4: return InputChannels::RGBA;
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