#include "stdafx.h"
#include "Texture.h"
namespace idk
{
	float Texture::AspectRatio() const
	{
		return s_cast<float>(_size.x) / _size.y;
	}
	uivec2 Texture::Size() const
	{
		return _size;
	}

	TextureInternalFormat Texture::InternalFormat() const
	{
		return _internal_format;
	}
	idk::FilterMode Texture::Filter() const
	{
		return _filter_mode;
	}
	bool Texture::IsDepthTexture() const
	{
		return _internal_format >= TextureInternalFormat::DEPTH_FIRST && _internal_format <= TextureInternalFormat::DEPTH_LAST;
	}
	uivec2 Texture::Size(uivec2 newsize)
	{
		return _size = newsize;
	}
}
