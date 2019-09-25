#include "pch.h"
#include <loading/OpenGLCubeMapLoader.h>
#include <gfx/GraphicsSystem.h>

#include <filesystem>

//Dep
#include <stb/stb_image.h>

//Resources
#include <idk_opengl/resource/OpenGLCubemap.h>
#include <gfx/CubeMap.h>

namespace idk
{
	ResourceBundle OpenGLCubeMapLoader::LoadFile(PathHandle path_to_resource)
	{//Assert for now
		assert(Core::GetSystem<GraphicsSystem>().GetAPI() == GraphicsAPI::OpenGL);

		auto texture_handle = Core::GetResourceManager().LoaderEmplaceResource<ogl::OpenGLCubemap>();

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

		for (int i = 0; i < fileExt->size(); ++i)
		{
			auto p = (path.parent_path() / cubemap.stem()).string() + fileExt[i] + ext.string();

			auto data = stbi_load(p.data(), &size.x, &size.y, &channels, 0);

			assert(data);

			texture_handle->Buffer(i, data, size, tm.internal_format);
		}

		return texture_handle;
	}

	ResourceBundle OpenGLCubeMapLoader::LoadFile(PathHandle path_to_resource, const MetaBundle& path_to_meta)
	{
		return ResourceBundle();
	}

};