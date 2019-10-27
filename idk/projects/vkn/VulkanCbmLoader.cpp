#include "pch.h"
#include "VulkanCbmLoader.h"
#include <vkn/VknCubemap.h>
#include <stb/stb_image.h>
#include <res/MetaBundle.h>
#include <vkn/VknCubemapLoader.h>
#include <sstream>
#include <filesystem>
#include <vkn/VulkanWin32GraphicsSystem.h>
namespace idk::vkn
{
	VulkanView& View();
	CbmLoader::CbmLoader() : allocator{ *View().Device(),View().PDevice() }
	{
		fence = View().Device()->createFenceUnique(vk::FenceCreateInfo{ vk::FenceCreateFlagBits::eSignaled });
	}
	vk::Format ToSrgb(vk::Format f);

	ResourceBundle CbmLoader::LoadFile(PathHandle handle, RscHandle<CubeMap> rtex, const CubeMapMeta* tm)
	{
		VknCubemap& tex = rtex.as<VknCubemap>();

		std::filesystem::path path{ handle.GetMountPath() };

		static string fileExt[] = {
			".r",
			".l",
			".t",
			".d",
			".b",
			".f"
		};

		auto cubemap = path.stem();
		auto ext = cubemap.extension();
		ivec2 size{};

		struct iData {
			ivec2 size;
			size_t strid;
			int n_chns;
			stbi_uc* data;
		};

		size_t strid = 0, t_size = 0;
		size_t v_size = std::size(fileExt);
		vector<stbi_uc*> loadList{ v_size };
		vector<size_t> strideList{ 0 };
		strideList.resize(6);
		for (int i = 0; i < v_size; ++i)
		{
			auto pp = (path.parent_path()).string() + "/" + (cubemap.stem()).string();
			auto p = pp + fileExt[i] + ext.string();
			PathHandle texPath{p};

			auto file = texPath.Open(FS_PERMISSIONS::READ, true);
			std::stringstream ss;
			ss << file.rdbuf();
			auto data = ss.str();
			int num_channels{};
			auto tex_data = stbi_load_from_memory(r_cast<const stbi_uc*>(data.data()), s_cast<int>(data.size()), &size.x, &size.y, &num_channels, 4);

			//loadList[i].size = size;
			//loadList[i].n_chns = num_channels;
			loadList[i] = tex_data;

			strideList[i] = size.x * size.y * 4;
			t_size += strideList[i];
		}

		CubemapLoader loader;
		CubemapOptions def{};
		std::optional<CubemapOptions> to{};
		if (tm)
			def = *(to = *tm);
		CMCreateInfo tci;
		tci.width = size.x;
		tci.height = size.y;
		tci.layout = vk::ImageLayout::eGeneral;
		tci.mipmap_level = 0;
		tci.sampled(true);
		tci.aspect = vk::ImageAspectFlagBits::eColor;
		tci.internal_format = vcm::MapFormat(def.internal_format);
		if (def.is_srgb)
			tci.internal_format = ToSrgb(tci.internal_format);
		InputCMInfo infoList;
		//TODO detect SRGB and load set format accordingly

		//Reassign the data to be held in one contigious memory
		char* something = s_cast<char*>(malloc(t_size));
		size_t offset = 0;
		for (int i = 0; i < v_size; ++i)
		{
			memcpy_s(something+offset,t_size-offset,loadList[i],strideList[i]);
			offset += strideList[i];
		}

		infoList = { r_cast<const char*>(something),t_size,strideList,def.is_srgb ? vk::Format::eR8G8B8A8Srgb : vk::Format::eR8G8B8A8Unorm };
		if (tm)
			to = *tm;
		loader.LoadCubemap(tex, allocator, *fence, to, tci, infoList);
		
		for(auto&elem: loadList)
			stbi_image_free(elem);

		free(something);
		return rtex;
	}

	ResourceBundle CbmLoader::LoadFile(PathHandle handle, const MetaBundle& meta)
	{
		auto tex = meta.CreateResource<VknCubemap>();
		return LoadFile(handle, RscHandle<CubeMap>{tex});
	}

}
