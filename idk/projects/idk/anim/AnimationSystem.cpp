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

	void AnimationSystem::Update(span<Animator> controllers)
	{
		// float a = Core::GetDT().count();

		for (auto& elem : controllers)
		{
			auto anim = elem.GetCurrentAnimation();
			const auto& skeleton = elem._skeleton->data();
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
				elem._elapsed += Core::GetRealDT().count();
				if (elem._elapsed >= anim->GetDuration())
					elem._elapsed -= anim->GetDuration();
				
				float ticks = elem._elapsed * anim->GetFPS();
				float num_ticks = anim->GetNumTicks();
				float time_in_ticks = fmod(ticks, num_ticks);

				// Loop through the skeleton
				for (size_t bone_id = 0; bone_id < skeleton.size(); ++bone_id)
				{
					auto& curr_go = elem._child_objects[bone_id];
					const auto& curr_bone = skeleton[bone_id];
					const anim::EasyAnimNode* anim_node = anim->GetEasyAnimNode(curr_bone._name);

					matrix_decomposition<real> curr_pose;
					bool first = true;
					if (anim_node != nullptr)
					{
						// For each channel starting from the front, we concatanate the transformations
						for (auto& channel : anim_node->_channels)
						{
							// Concat the interpolated channels and the current pose.
							auto interp_pose = interpolateChannel(channel, time_in_ticks);
							if (first)
							{
								curr_pose.position =	interp_pose.position;
								curr_pose.rotation =	interp_pose.rotation;
								curr_pose.scale =		interp_pose.scale;
								first = false;
								continue;
							}
							curr_pose.position = curr_pose.position + interp_pose.position;
							curr_pose.rotation = curr_pose.rotation	* interp_pose.rotation;
							curr_pose.scale =	 curr_pose.scale	+ interp_pose.scale;
						}
					}
					else
					{
						// For now, bones and anim_nodes are 1-1.
						assert(false);
					}
					
					const auto& curr_local_bind_pose = elem._bind_pose[bone_id];
					curr_pose.position =	curr_local_bind_pose.position	+ curr_pose.position;
					curr_pose.rotation =	curr_local_bind_pose.rotation	* curr_pose.rotation;
					curr_pose.scale =		curr_local_bind_pose.scale		+ curr_pose.scale;
					// mat4 compose_curr_pose = curr_pose.recompose();

					// During GenerateTransforms in the Animator, it will use the child transforms to 
					// generate the final transforms
					curr_go->Transform()->position = curr_pose.position;
					curr_go->Transform()->rotation = curr_pose.rotation;
					curr_go->Transform()->scale = curr_pose.scale;
					
				}
			}
		}
	}

	void AnimationSystem::Shutdown()
	{
	}

	matrix_decomposition<real> AnimationSystem::interpolateChannel(const anim::Channel& channel, float time_in_ticks)
	{
		matrix_decomposition<real> interp_pose;
		interp_pose.scale = channel._scale.size() == 0			? vec3{} : channel._scale[0]._val;
		interp_pose.rotation = channel._rotation.size() == 0	? quat{} : channel._rotation[0]._val;
		interp_pose.position = channel._translate.size() == 0	? vec3{} : channel._translate[0]._val;

		// Scaling
		if (channel._scale.size() > 1)
		{
			size_t start = find_key(channel._scale, time_in_ticks);
			if (start + 1 >= channel._scale.size())
			{
				interp_pose.scale = channel._scale[start]._val;
			}
			else
			{
				float dt = static_cast<float>(channel._scale[start + 1]._time - channel._scale[start]._time);
				float factor = (time_in_ticks - channel._scale[start]._time) / dt;
				assert(factor >= 0.0f && factor <= 1.0f);

				interp_pose.scale = lerp(channel._scale[start]._val, channel._scale[start + 1]._val, factor);
			}
		}

		// Translate
		if (channel._translate.size() > 1)
		{
			size_t start = find_key(channel._translate, time_in_ticks);
			if (start + 1 >= channel._translate.size())
			{
				interp_pose.position = channel._translate[start]._val;
			}
			else
			{
				float dt = static_cast<float>(channel._translate[start + 1]._time - channel._translate[start]._time);
				float factor = (time_in_ticks - channel._translate[start]._time) / dt;
				assert(factor >= 0.0f && factor <= 1.0f);

				interp_pose.position = lerp(channel._translate[start]._val, channel._translate[start + 1]._val, factor);
			}
		}

		// Rotate
		if (channel._rotation.size() > 1)
		{
			size_t start = find_key(channel._rotation, time_in_ticks);
			if (start + 1 >= channel._rotation.size())
			{
				interp_pose.rotation = channel._rotation[start]._val;
			}
			else
			{
				float dt = static_cast<float>(channel._rotation[start + 1]._time - channel._rotation[start]._time);
				float factor = (time_in_ticks - channel._rotation[start]._time) / dt;
				assert(factor >= 0.0f && factor <= 1.0f);

				interp_pose.rotation = slerp(channel._rotation[start]._val, channel._rotation[start + 1]._val, factor);
				// rotation =  * rotation;
				interp_pose.rotation.normalize();
			}
		}
		
		return interp_pose;
	}
}