#include "pch.h"
#include "VknTextureData.h"

#include <vkn/VknTexture.h>

namespace idk::vkn
{

void VknTextureData::ApplyOnTexture(VknTexture& tex)
{
	tex.Size(size);
	tex.image_	      = std::move(image_);
	tex.format	      = std::move(format);
	tex.usage	      = std::move(usage);
	tex.img_aspect    = std::move(img_aspect);
	tex.mem		      = std::move(mem);
	tex.mem_alloc     = std::move(mem_alloc);
	tex.imageView     = std::move(imageView);
	tex.sampler	      = std::move(sampler);
	tex.descriptorSet = std::move(descriptorSet);
	tex.range         = std::move(range);
	tex.mipmap_level  = std::move(mipmap_level);
	tex.dbg_name	  = std::move(tex.dbg_name);
	tex.image_type	  = std::move(image_type);
	tex.Layers(layers);
}

vk::Image VknTextureData::Image(bool) const
{
	return *image_;
}

void VknTextureData::Size(uvec2 sz)
{
	size = sz;
}

void VknTextureData::Layers(uint32_t l)
{
	layers = l;
}

string_view VknTextureData::Name()
{
	return dbg_name;
}

}