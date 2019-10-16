#pragma once
#include <res/ResourceHandle.h>
namespace idk
{
	namespace anim { class Animation; }
	struct AnimationState
	{
		RscHandle<anim::Animation> animation{};
		float speed = 1.0f;
	};
}