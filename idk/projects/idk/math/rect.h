#pragma once
#include <idk.h>

namespace idk
{
	struct rect
	{
        vec2 position{ 0, 0 };
        vec2 size{ 1.0f, 1.0f };

		template<typename T>
		rect& Scale(tvec<T, 2> scale)noexcept
		{
			position *= vec2{ scale };
			size *= vec2{ scale };
			return *this;
		}
		template<typename T>
		rect Scaled(tvec<T, 2> scale)noexcept
		{
			rect tmp = *this;
			tmp.Scale(scale);
			return tmp;
		}
	};
}