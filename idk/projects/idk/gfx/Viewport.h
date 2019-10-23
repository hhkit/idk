#pragma once
#include <idk.h>

namespace idk
{
	struct Viewport
	{
		vec2 position{ 0,0 }, size{ 1,1 };//(0,1)
	};
}