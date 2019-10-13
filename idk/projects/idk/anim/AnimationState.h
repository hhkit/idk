#pragma once
#include <anim/Animation.h>

namespace idk
{
	struct AnimationState
	{
		RscHandle<anim::Animation> animation{};
		float speed = 1.0f;
	};
}