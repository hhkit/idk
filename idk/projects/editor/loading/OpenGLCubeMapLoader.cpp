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
	FileResources OpenGLCubeMapLoader::Create(PathHandle path_to_resource)
	{	
		//Assert for now
		assert(Core::GetSystem<GraphicsSystem>().GetAPI() == GraphicsAPI::OpenGL);

		auto texture_handle = Core::GetResourceManager().Create<CubeMap>();

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

		for (int i=0; i< fileExt->size(); ++i)
		{
			auto p = (path.parent_path() / cubemap.stem()).string() + fileExt[i] + ext.string();

			auto data = stbi_load(p.data(), &size.x, &size.y, &channels, 0);

			assert(data);

			texture_handle.as<ogl::OpenGLCubemap>().Buffer(i,data, size, tm.internal_format);
		}


		FileResources retval;


		retval.resources.emplace_back(texture_handle);

		return retval;
	}

	FileResources OpenGLCubeMapLoader::Create(PathHandle path_to_resource, const MetaFile& path_to_meta)
	{
		UNREFERENCED_PARAMETER(path_to_resource);
		UNREFERENCED_PARAMETER(path_to_meta);

		return Create(path_to_resource);
	}

};