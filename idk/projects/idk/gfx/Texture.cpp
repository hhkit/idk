#include "stdafx.h"
#include "Texture.h"
namespace idk
{
	float Texture::AspectRatio() const
	{
		return s_cast<float>(meta.size.x) / meta.size.y;
	}
	ivec2 Texture::Size() const
	{
		return _size;
	}
	void Texture::Size(ivec2 newsize)
	{
		_size = newsize;
	}
}
