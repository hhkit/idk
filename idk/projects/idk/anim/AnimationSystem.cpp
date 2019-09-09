#include "stdafx.h"
#include "AnimationSystem.h"

#include <core/GameObject.h>
#include <common/Transform.h>

namespace idk
{
	void AnimationSystem::Init()
	{
	}

	void AnimationSystem::Update(span<AnimationController> controllers)
	{
		// float a = Core::GetDT().count();

		for (auto elem : controllers)
		{
			auto anim = elem.GetCurrentAnimation();
			if (elem._is_playing && anim)
			{
				// Update the components here:
				// 1) Loop through bones.
				// 2) Check if current bone has animated node or not.
				//		- YES: parent_transform = parent_transform * INTERPOLATED_transform.
				//		- NO:  parent_transform = parent_transform * NODE_transform.
				// 3) Concat the game object's current transform with the confirmed bone transform. Can subscript directly into game object
				const auto& skeleton = elem._skeleton->data();
				// const mat4& glob_inverse = elem._skeleton->GetGlobalInverse();

				// We keep all the bone transforms first. Easier to refer to parent's transform, fewer mat mults.
				vector<mat4> bone_transforms;
				bone_transforms.reserve(skeleton.size());

				for (size_t bone_id = 0; bone_id < skeleton.size(); ++bone_id)
				{
					const auto& curr_bone = skeleton[bone_id];

					const anim::Animation::AnimNode* anim_node = anim->GetAnimNode(curr_bone._name);

					mat4 curr_transform = curr_bone._node_transform;
					if (anim_node != nullptr)
					{
						// Interpolate here
					}
					
					// if (curr_bone._parent < 0)
						// local transform = curr_transform * bone_offset
					// else
						// local transform = parent_bone_offset.inverse() * curr_transform * bone_offset
				}

				for (size_t xform_id = 0; xform_id < bone_transforms.size(); ++xform_id)
					elem._child_objects[xform_id]->GetComponent<Transform>()->GlobalMatrix(bone_transforms[xform_id]);

			}
		}
	}

	void AnimationSystem::Shutdown()
	{
	}
}