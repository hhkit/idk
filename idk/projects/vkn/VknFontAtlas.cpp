#include "pch.h"

#include <vkn/VknFontAtlas.h>
#include <res/ResourceManager.inl>
#include <res/ResourceHandle.inl>
#include <vkn/TextureTracker.h>

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