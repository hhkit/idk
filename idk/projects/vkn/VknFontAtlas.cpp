#include "pch.h"

#include <vkn/VknFontAtlas.h>
#include <res/ResourceManager.inl>
#include <res/ResourceHandle.inl>
#include <vkn/TextureTracker.h>
#include <vkn/VulkanFontAtlasFactory.h>

namespace idk::vkn {
	RscHandle<Texture> VknFontAtlas::Tex() const noexcept
	{
		return RscHandle<Texture>{texture};
	}
	vk::ImageAspectFlags VknFontAtlas::ImageAspects()
	{
		return texture->img_aspect;
	}
	VknFontAtlas::~VknFontAtlas()
	{
		if (texture)
			dbg::TextureTracker::Inst(dbg::TextureAllocTypes::eCubemap).reg_deallocate(texture.as<VknTexture>().Image().operator VkImage());
		Core::GetResourceManager().Release(texture);
	}
	VknFontAtlas::VknFontAtlas(VknFontAtlas&& rhs) noexcept
		:FontAtlas{ std::move(rhs) },
		texture{rhs.texture}
	{
		rhs.texture = RscHandle<VknTexture>{};
	}
	VknFontAtlas::VknFontAtlas(const CompiledFontAtlas& compiled_font)
	{
		if (compiled_font.pixel_buffer.size() == 0)
			return;
		string_view data = { reinterpret_cast<const char*>(compiled_font.pixel_buffer.data()), compiled_font.pixel_buffer.size() };

		auto& loader = Core::GetResourceManager().GetFactory<VulkanFontAtlasFactory>().GetIttfLoader();

		_size = compiled_font.size;
		char_map.resize(128);
		std::copy(std::begin(compiled_font.char_map), std::end(compiled_font.char_map), std::begin(char_map));
		ascender = compiled_font.ascender;
		descender = compiled_font.descender;

		loader.LoadFontAtlas(*this, data, compiled_font);
	}

	VknFontAtlas& VknFontAtlas::operator=(VknFontAtlas&& rhs) noexcept
	{
		FontAtlas::operator=(std::move(rhs));
		std::swap(texture, rhs.texture);


		return *this;
	}
	void VknFontAtlas::Size(uvec2 new_size)
	{
		FontAtlas::Size(new_size);

		//ToDO update the size of texture
	}
	void* VknFontAtlas::ID() const
	{
		//Should be descriptor set 
		return texture->ID();
	}
	void VknFontAtlas::OnMetaUpdate(const FontAtlasMeta&)
	{
		//Update meta "does nothing now"
	}
	void VknFontAtlas::UpdateUV(UVMode)
	{

	}
}