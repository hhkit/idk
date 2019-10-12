#include "pch.h"
#include "VulkanPngLoader.h"
#include <vkn/VknTexture.h>
#include <stb/stb_image.h>
#include <res/MetaBundle.h>
#include <vkn/VknTextureLoader.h>
#include <sstream>
#include <vkn/VulkanWin32GraphicsSystem.h>
namespace idk::vkn
{
	VulkanView& View();
	PngLoader::PngLoader(): allocator{*View().Device(),View().PDevice()}
	{
		fence = View().Device()->createFenceUnique(vk::FenceCreateInfo{ vk::FenceCreateFlagBits::eSignaled });
	}
	ResourceBundle PngLoader::LoadFile(PathHandle handle, RscHandle<Texture> rtex, const TextureMeta* tm)
	{
		VknTexture& tex = rtex.as<VknTexture>();
		auto file = handle.Open(FS_PERMISSIONS::READ, true);
		std::stringstream ss;
		ss << file.rdbuf();
		auto data = ss.str();
		ivec2 size{};
		int num_channels{};
		auto tex_data = stbi_load_from_memory(r_cast<const stbi_uc*>(data.data()), s_cast<int>(data.size()), &size.x, &size.y, &num_channels, 4);
		TextureLoader loader;
		std::optional<TextureOptions> to{};
		TexCreateInfo tci;
		tci.width = size.x;
		tci.height = size.y;
		tci.layout = vk::ImageLayout::eGeneral;
		tci.mipmap_level = 0;
		tci.sampled(true);
		tci.aspect = vk::ImageAspectFlagBits::eColor;
		tci.internal_format = MapFormat(to->internal_format);
		//TODO detect SRGB and load set format accordingly
		InputTexInfo iti{ r_cast<const char*>(tex_data),s_cast<size_t>(size.x * size.y * 4),vk::Format::eR8G8B8A8Srgb };
		if(tm)
			to= *tm;
		loader.LoadTexture(tex, allocator, *fence,to, tci, iti);
		stbi_image_free(tex_data);
		return rtex;
	}
	ResourceBundle idk::vkn::PngLoader::LoadFile(PathHandle handle)
	{
		auto tex = Core::GetResourceManager().LoaderEmplaceResource<VknTexture>();

		return LoadFile(handle, RscHandle<Texture>{ tex });;
	}

	ResourceBundle idk::vkn::PngLoader::LoadFile(PathHandle handle, const MetaBundle& meta)
	{
		auto tex = Core::GetResourceManager().LoaderEmplaceResource<VknTexture>(meta.metadatas[0].guid);
		return LoadFile(handle, RscHandle<Texture>{tex});;
	}

}
