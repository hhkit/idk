#include "stdafx.h"
#include "AnimationSystem.h"

namespace idk
{
	void AnimationSystem::Init()
	{
	}

	void AnimationSystem::Update(span<AnimationController> controllers)
	{
		for (auto elem : controllers)
		{
			if (elem._is_playing)
			{
				// Update the components here:
				// 1) Loop through bones.
				// 2) Check if current bone has animated node or not.
				//		- YES: parent_transform = parent_transform * INTERPOLATED_transform.
				//		- NO:  parent_transform = parent_transform * NODE_transform.
				// 3) Concat the game object's current transform with the confirmed bone transform. Can subscript directly into game object
			}
		}
	}

	void AnimationSystem::Shutdown()
	{
	}
}