#include "stdafx.h"
#include "Texture.h"
namespace idk
{
	float Texture::AspectRatio() const
	{
		return s_cast<float>(_size.x) / _size.y;
	}
	ivec2 Texture::Size() const
	{
		return _size;
	}
	ivec2 Texture::Size(ivec2 newsize)
	{
		return _size = newsize;
	}
}
