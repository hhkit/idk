#include "stdafx.h"
#include "CubeMap.h"

namespace idk {
	float CubeMap::AspectRatio() const
	{
		return s_cast<float>(_size.x) / _size.y;
	}
	uvec2 CubeMap::Size() const
	{
		return _size;
	}
	void CubeMap::Size(uvec2 newsize)
	{
		_size = newsize;
	}
};
