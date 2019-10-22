#pragma once
#include <res/ResourceHandle.h>
namespace idk
{
	namespace anim { class Animation; }
	struct AnimationState
	{
		string name{};
		RscHandle<anim::Animation> animation{};
		bool enabled = false;
		bool loop = true;
		float speed = 1.0f;
	};
}