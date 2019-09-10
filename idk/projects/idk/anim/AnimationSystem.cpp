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
				
				const mat4& glob_inverse = elem._skeleton->GetGlobalInverse();

				// We keep all the bone transforms first. Easier to refer to parent's transform, fewer mat mults.
				vector<mat4> bone_transforms;
				bone_transforms.reserve(skeleton.size());

				// Compute the time
				elem._elapsed += Core::GetRealDT().count();
				if (elem._elapsed >= anim->GetDuration())
					elem._elapsed -= anim->GetDuration();
				
				float ticks = elem._elapsed * anim->GetFPS();
				float time_in_ticks = fmod(ticks, anim->GetNumTicks());

				for (size_t bone_id = 0; bone_id < skeleton.size(); ++bone_id)
				{
					const auto& curr_bone = skeleton[bone_id];
					
					const anim::Animation::AnimNode* anim_node = anim->GetAnimNode(curr_bone._name);

					mat4 local_bone_transform;
					if (anim_node != nullptr)
					{
						
						// For each channel starting from the front, we concatanate the transformations
						;
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
					
					if (curr_bone._parent < 0)
						bone_transforms.push_back(glob_inverse * local_bone_transform * curr_bone._offset);
					else
					{
						const auto& parent_bone = skeleton[curr_bone._parent];
						bone_transforms.push_back(parent_bone._offset.inverse() * local_bone_transform * curr_bone._offset);
					}
				}

				for (size_t xform_id = 0; xform_id < bone_transforms.size(); ++xform_id)
					elem._child_objects[xform_id]->GetComponent<Transform>()->LocalMatrix(bone_transforms[xform_id]);
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
			float factor = time_in_ticks - (channel._scale[start]._time / dt);
			assert(factor >= 0.0f && factor <= 1.0f);

			scale = lerp(channel._scale[start]._val, channel._scale[start + 1]._val, factor);
		}

		// Translate
		if (channel._translate.size() > 1)
		{
			size_t start = find_key(channel._translate, time_in_ticks);
			assert(start + 1 < channel._translate.size());

			float dt = static_cast<float>(channel._translate[start + 1]._time - channel._translate[start]._time);
			float factor = time_in_ticks - (channel._translate[start]._time / dt);
			assert(factor >= 0.0f && factor <= 1.0f);

			translation = lerp(channel._translate[start]._val, channel._translate[start + 1]._val, factor);
		}

		// Rotate
		if (channel._rotation.size() > 1)
		{
			size_t start = find_key(channel._rotation, time_in_ticks);
			assert(start + 1 < channel._rotation.size());

			float dt = static_cast<float>(channel._rotation[start + 1]._time - channel._rotation[start]._time);
			float factor = time_in_ticks - (channel._rotation[start]._time / dt);
			assert(factor >= 0.0f && factor <= 1.0f);

			auto ohm = acos(channel._rotation[start]._val.get_normalized().dot(channel._rotation[start + 1]._val.get_normalized()));
			constexpr auto _1 = 1.0f;
			auto t = sin(ohm * (_1 - 1.0f)) / sin(ohm) * channel._rotation[start]._val + sin(ohm * 1.0f) / sin(ohm) * channel._rotation[start + 1]._val;
			// rotation = slerp(channel._rotation[start]._val, channel._rotation[start + 1]._val, factor);
			rotation = quat{ t.w, t.x, t.y, t.z };
			rotation.normalize();
		}
		

		return translate(translation) * mat4 { quat_cast<mat3>(rotation)* idk::scale(scale) };
	}
}