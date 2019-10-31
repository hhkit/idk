#include "stdafx.h"
#include "FontAtlas.h"

namespace idk {
	float FontAtlas::AspectRatio() const
	{
		return s_cast<float>(_size.x) / _size.y;
	}
	ivec2 FontAtlas::Size() const
	{
		return _size;
	}
	void FontAtlas::Size(ivec2 newsize)
	{
		_size = newsize;
	}
}
