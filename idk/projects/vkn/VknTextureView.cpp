#include "pch.h"
#include "VknTextureView.h"
#include <vkn/VknTexture.h>
#include <vkn/VknCubemap.h>

namespace idk::vkn
{
	VknTextureView::VknTextureView(const VknTexture& tex) noexcept:
		 _image     {tex.Image()}
		,_image_view{tex.ImageView()}
		,_sampler   {tex.Sampler()}
		,_size      {tex.Size()}
		,_format    {tex.format}
		,_aspects   {tex.ImageAspects()}
		,_layers    {tex.Layers()}
		,_range     {tex.FullRange()}
	{
	}
	VknTextureView::VknTextureView(const VknCubemap& tex) noexcept:
		 _image     {tex.Image()}
		,_image_view{tex.ImageView()}
		,_sampler   {tex.Sampler()}
		,_size      {tex.Size()}
		,_format    {tex.Format()}
		,_aspects   {tex.Aspect()}
		,_layers    {tex.Layers()}
	{
	}
	VknTextureView& VknTextureView::operator=(const VknTexture& tex) noexcept
	{
		this->~VknTextureView();
		new (this) VknTextureView(tex);
		return *this;
	}
	VknTextureView& VknTextureView::operator=(const VknCubemap& tex) noexcept
	{
		this->~VknTextureView();
		new (this) VknTextureView(tex);
		return *this;
	}
	vk::Image VknTextureView::Image() const noexcept
	{
		return _image;
	}
	vk::ImageView VknTextureView::ImageView() const noexcept
	{
		return _image_view;
	}
	vk::Sampler VknTextureView::Sampler() const noexcept
	{
		return _sampler;
	}
	uvec2 VknTextureView::Size() const noexcept
	{
		return _size;
	}
	vk::Format VknTextureView::Format() const noexcept
	{
		return _format;
	}
	vk::ImageAspectFlags VknTextureView::ImageAspects() const noexcept
	{
		return _aspects;
	}
	uint32_t VknTextureView::Layers() const noexcept
	{
		return _layers;
	}
	vk::ImageSubresourceRange VknTextureView::FullRange() const noexcept
	{
		return _range;
	}
}