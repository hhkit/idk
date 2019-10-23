#pragma once
#include <idk.h>

namespace idk
{
	struct Viewport
	{
		vec2 min{ 0,0 }, max{ 1,1 };//(0,1)
	};
}