#include "pch.h"

#include <vkn/VknFontAtlas.h>

namespace idk::vkn {
	vk::ImageAspectFlags VknFontAtlas::ImageAspects()
	{
		return img_aspect;
	}
	VknFontAtlas::~VknFontAtlas()
	{
	}
	VknFontAtlas::VknFontAtlas(VknFontAtlas&& rhs) noexcept
		:FontAtlas{ std::move(rhs) },
		size{ std::move(rhs.size) },
		sizeOnDevice{ std::move(rhs.sizeOnDevice) },
		rawData{ std::move(rhs.rawData) },
		path{ std::move(rhs.path) },
		image{ std::move(rhs.image) },
		format{ std::move(rhs.format) },
		mem{ std::move(rhs.mem) },
		imageView{ std::move(rhs.imageView) },
		sampler{ std::move(rhs.sampler) }
	{
		
	}
	VknFontAtlas& VknFontAtlas::operator=(VknFontAtlas&& rhs) noexcept
	{
		// TODO: insert return statement here
		FontAtlas::operator=(std::move(rhs));

		std::swap(size, rhs.size);
		std::swap(sizeOnDevice, rhs.sizeOnDevice);
		std::swap(rawData, rhs.rawData);
		std::swap(path, rhs.path);
		std::swap(image, rhs.image);
		std::swap(format, rhs.format);
		std::swap(mem, rhs.mem);
		std::swap(imageView, rhs.imageView);
		std::swap(sampler, rhs.sampler);

		return *this;
	}
	void VknFontAtlas::Size(ivec2 new_size)
	{
		FontAtlas::Size(new_size);

		//ToDO update the size of texture
	}
	void* VknFontAtlas::ID() const
	{
		//Should be descriptor set 
		return r_cast<void*>(imageView->operator VkImageView());
	}
	void VknFontAtlas::OnMetaUpdate(const FontAtlasMeta&)
	{
		//Update meta "does nothing now"
	}
	void VknFontAtlas::UpdateUV(UVMode)
	{

	}
}