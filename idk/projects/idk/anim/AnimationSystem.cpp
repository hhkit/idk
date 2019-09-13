#include "stdafx.h"
#include "AnimationSystem.h"

#include <core/GameObject.h>
#include <common/Transform.h>
#include <math/arith.h>
#include <math/matrix_transforms.h>

namespace idk
{
	void AnimationSystem::Init()
	{
	}

	void AnimationSystem::Update(span<AnimationController> controllers)
	{
		// float a = Core::GetDT().count();

		for (auto& elem : controllers)
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
				

				// We keep all the bone transforms first. Easier to refer to parent's transform, fewer mat mults.
				
				// Compute the time
				elem._elapsed += Core::GetDT().count();
				if (elem._elapsed >= anim->GetDuration())
					elem._elapsed -= anim->GetDuration();
				
				float ticks = elem._elapsed * anim->GetFPS();
				float num_ticks = anim->GetNumTicks();
				float time_in_ticks = fmod(ticks, num_ticks);

				// Loop through the skeleton
				const auto& skeleton = elem._skeleton->data();
				for (size_t bone_id = 0; bone_id < skeleton.size(); ++bone_id)
				{
					const auto& curr_bone = skeleton[bone_id];
					const anim::Animation::EasyAnimNode* anim_node = anim->GetEasyAnimNode(curr_bone._name);

					mat4 local_bone_transform;
					if (anim_node != nullptr)
					{
						// For each channel starting from the front, we concatanate the transformations
						for (auto& channel : anim_node->_channels)
						{
							if (channel._is_animated)
							{
								// Interpolate here
								local_bone_transform = local_bone_transform * interpolateChannel(channel, time_in_ticks);
							}
							else
							{
								local_bone_transform = local_bone_transform * channel._node_transform;
							}
						}
					}
					else
					{
						// For now, bones and anim_nodes are 1-1.
						assert(false);
					}
					elem._bone_transforms[bone_id] = local_bone_transform;
				}
			}
		}
	}

	void AnimationSystem::Shutdown()
	{
	}
	mat4 AnimationSystem::interpolateChannel(const anim::Animation::Channel& channel, float time_in_ticks)
	{
		vec3 scale = channel._scale[0]._val;
		quat rotation = channel._rotation[0]._val;
		vec3 translation = channel._translate[0]._val;

		// Scaling
		if (channel._scale.size() > 1)
		{
			size_t start = find_key(channel._scale, time_in_ticks);
			assert(start + 1 < channel._scale.size());

			float dt = static_cast<float>(channel._scale[start + 1]._time - channel._scale[start]._time);
			float factor = (time_in_ticks - channel._scale[start]._time) / dt;
			assert(factor >= 0.0f && factor <= 1.0f);

			scale = lerp(channel._scale[start]._val, channel._scale[start + 1]._val, factor);
		}

		// Translate
		if (channel._translate.size() > 1)
		{
			size_t start = find_key(channel._translate, time_in_ticks);
			assert(start + 1 < channel._translate.size());

			float dt = static_cast<float>(channel._translate[start + 1]._time - channel._translate[start]._time);
			float factor = (time_in_ticks - channel._translate[start]._time) / dt;
			assert(factor >= 0.0f && factor <= 1.0f);

			translation = lerp(channel._translate[start]._val, channel._translate[start + 1]._val, factor);
		}

		// Rotate
		if (channel._rotation.size() > 1)
		{
			size_t start = find_key(channel._rotation, time_in_ticks);
			assert(start + 1 < channel._rotation.size());

			float dt = static_cast<float>(channel._rotation[start + 1]._time - channel._rotation[start]._time);
			float factor = (time_in_ticks - channel._rotation[start]._time) / dt;
			assert(factor >= 0.0f && factor <= 1.0f);

			rotation = slerp(channel._rotation[start]._val, channel._rotation[start + 1]._val, factor);
			rotation.normalize();
		}
		
		auto test = quat_cast<mat3>(quat{ 1, 0,0 ,0 });
		return translate(translation) * mat4 { quat_cast<mat3>(rotation)* idk::scale(scale) };
	}
}