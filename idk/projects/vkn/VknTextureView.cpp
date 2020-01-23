#include "pch.h"
#include "VknTextureView.h"

namespace idk::vkn
{
	VknTextureView::VknTextureView(const VknTexture& tex) noexcept:
		 _image     {tex.Image()}
		,_image_view{tex.ImageView()}
		,_sampler   {tex.Sampler()}
		,_size      {tex.Size()}
		,_format    {tex.format}
		,_aspects   {tex.img_aspect}
	{
	}
	VknTextureView& VknTextureView::operator=(const VknTexture& tex) noexcept
	{
		this->~VknTextureView();
		new (this) VknTextureView(tex);
		return *this;
	}
	vk::Image VknTextureView::Image() const
	{
		return _image;
	}
	vk::ImageView VknTextureView::ImageView() const
	{
		return _image_view;
	}
	vk::Sampler VknTextureView::Sampler() const
	{
		return _sampler;
	}
	uivec2 VknTextureView::Size() const
	{
		return _size;
	}
	vk::Format VknTextureView::Format() const
	{
		return _format;
	}
	vk::ImageAspectFlags VknTextureView::ImageAspects() const
	{
		return _aspects;
	}
}