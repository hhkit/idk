#include "pch.h"
#include <vkn/VknFontAtlasLoader.h>
#include <res/MetaBundle.h>
#include <sstream>
#include <filesystem>
#include <vkn/VulkanWin32GraphicsSystem.h>

#include <vkn/VknFontAtlas.h>

namespace idk::vkn
{
	VknFontAtlasLoader::VknFontAtlasLoader()
		: allocator{ *View().Device(),View().PDevice() }
	{
		fence = View().Device()->createFenceUnique(vk::FenceCreateInfo{ vk::FenceCreateFlagBits::eSignaled });
	}
	ResourceBundle VknFontAtlasLoader::LoadFile(PathHandle handle, RscHandle<FontAtlas> fa, const FontAtlasMeta* tm)
	{
		VknFontAtlas& fontAtlas = fa.as<VknFontAtlas>();

		std::filesystem::path path{ handle.GetMountPath() };
		return ResourceBundle();
	}
	ResourceBundle VknFontAtlasLoader::LoadFile(PathHandle handle, const MetaBundle& meta)
	{
		return ResourceBundle();
	}
}