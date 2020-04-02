#include "pch.h"
#include "VulkanIttfLoader.h"
//#include <vkn/VknFontAtlas.h>
#include <res/MetaBundle.inl>
#include <sstream>
#include <vkn/VulkanWin32GraphicsSystem.h>
#include <res/ResourceManager.inl>
#include <res/ResourceHandle.inl>
#include <res/ResourceMeta.inl>

//Resources
#include <iostream>

namespace idk::vkn
{
	VulkanView& View();
	IttfLoader::IttfLoader() : allocator{ *View().Device(),View().PDevice() }
	{
		fence = View().Device()->createFenceUnique(vk::FenceCreateInfo{ vk::FenceCreateFlagBits::eSignaled });
	}
	vk::Format ToSrgb(vk::Format f);

	void IttfLoader::LoadFontAtlas(VknFontAtlas& font_handle, string_view file, const FontAtlasOptions& tm)
	{
		//Loading ttf files with raw pixel buffer instead to remove dependecies on freetype
		
		//ASSUMPTION: meta file exist
		uvec2 size = font_handle.Size();
		
		//string_view raw = file.data();
		FontAtlasLoader loader;
		FontAtlasOptions def{};
		std::optional<FontAtlasOptions> to{};
		//if (tm)
			//def = *(to = *tm);
		def = tm;

		FACreateInfo tci;
		tci.width = size.x;
		tci.height = size.y;
		tci.layout = vk::ImageLayout::eGeneral;
		tci.mipmap_level = 0;
		tci.sampled(true);
		tci.aspect = vk::ImageAspectFlagBits::eColor;
		tci.internal_format = MapFormat(ToInternalFormat(def.internal_format, def.is_srgb));
		if (def.is_srgb)
			tci.internal_format = ToSrgb(tci.internal_format);
		//TODO detect SRGB and load set format accordingly
		InputFAInfo iti{ file.data(),size.x * size.y,def.is_srgb ? vk::Format::eR8Srgb : vk::Format::eR8Uint };
		//if (tm)
		to = tm;
		loader.LoadFontAtlas(font_handle, allocator, *fence, to, tci, iti);

		//font_handle = *fontAtlas;

		//return fontAtlas;
	}

	ResourceBundle IttfLoader::LoadFile(PathHandle handle, const MetaBundle& meta)
	{
		auto m = meta.FetchMeta<FontAtlas>();
		auto tex = m ? Core::GetResourceManager().LoaderEmplaceResource<VknFontAtlas>(m->guid)
			: Core::GetResourceManager().LoaderEmplaceResource<VknFontAtlas>();
		//auto tex = meta.CreateResource<VknFontAtlas>();
		auto file = handle.Open(idk::FS_PERMISSIONS::READ, true);

		std::stringstream strm;
		strm << file.rdbuf();
		FontAtlasOptions to;
		if (m)
		{
			auto load = m->GetMeta<FontAtlas>();
			if (load)
				to = *load;
		}
		LoadFontAtlas(*tex, strm.str(), to);
		return tex;
	}

}
