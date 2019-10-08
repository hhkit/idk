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

	void AnimationSystem::Update(span<Animator> animators)
	{
		AnimationPass(animators);
		// Blend pass
		// Mix pass
	}

	void AnimationSystem::Shutdown()
	{
	}

	void AnimationSystem::AnimationPass(span<Animator> animators)
	{
		for (auto& elem : animators)
		{
			const auto anim = elem.GetCurrentAnimation();
			const auto& skeleton = elem._skeleton->data();
			if (elem._is_playing && anim)
			{
				// Compute the time
				elem._elapsed += Core::GetRealDT().count();
				if (elem._elapsed >= anim->GetDuration())
					elem._elapsed -= anim->GetDuration();

				const float ticks = elem._elapsed * anim->GetFPS();
				const float num_ticks = anim->GetNumTicks();
				const float time_in_ticks = fmod(ticks, num_ticks);

				// Loop through the skeleton
				for (size_t bone_id = 0; bone_id < skeleton.size(); ++bone_id)
				{
					const auto& curr_bone = skeleton[bone_id];
					const anim::AnimatedBone* animated_bone = anim->GetAnimatedBone(curr_bone._name);

					auto& curr_go = elem._child_objects[bone_id];
					if (!curr_go || animated_bone == nullptr)
						continue;	

					matrix_decomposition<real> curr_pose = elem._bind_pose[bone_id];
					// Interpolate function will fill in the curr_pose as needed. If there are no keys, it will keep it in bind pose.
					InterpolateBone(*animated_bone, time_in_ticks, curr_pose);
					
					// During GenerateTransforms in the Animator, it will use the child transforms to 
					// generate the final transforms
					curr_go->Transform()->position = curr_pose.position;
					curr_go->Transform()->rotation = curr_pose.rotation;
					curr_go->Transform()->scale = curr_pose.scale;

				}
			}
		}
	}

	void AnimationSystem::InterpolateBone(const anim::AnimatedBone& animated_bone, float time_in_ticks, matrix_decomposition<real>& curr_pose)
	{
		// Scaling
		if (animated_bone.scale_track.size() > 1)
		{
			const size_t start = find_key(animated_bone.scale_track, time_in_ticks);
			if (start + 1 >= animated_bone.scale_track.size())
			{
				curr_pose.scale = animated_bone.scale_track[start].val;
			}
			else
			{
				const float dt = static_cast<float>(animated_bone.scale_track[start + 1].time - animated_bone.scale_track[start].time);
				const float factor = (time_in_ticks - animated_bone.scale_track[start].time) / dt;
				assert(factor >= 0.0f && factor <= 1.0f);

				curr_pose.scale = lerp(animated_bone.scale_track[start].val, animated_bone.scale_track[start + 1].val, factor);
			}
		}

		// Translate
		if (animated_bone.translate_track.size() > 1)
		{
			const size_t start = find_key(animated_bone.translate_track, time_in_ticks);
			if (start + 1 >= animated_bone.translate_track.size())
			{
				curr_pose.position = animated_bone.translate_track[start].val;
			}
			else
			{
				const float dt = static_cast<float>(animated_bone.translate_track[start + 1].time - animated_bone.translate_track[start].time);
				const float factor = (time_in_ticks - animated_bone.translate_track[start].time) / dt;
				assert(factor >= 0.0f && factor <= 1.0f);

				curr_pose.position = lerp(animated_bone.translate_track[start].val, animated_bone.translate_track[start + 1].val, factor);
			}
		}

		// Rotate
		if (animated_bone.rotation_track.size() > 1)
		{
			const size_t start = find_key(animated_bone.rotation_track, time_in_ticks);
			if (start + 1 >= animated_bone.rotation_track.size())
			{
				curr_pose.rotation = animated_bone.rotation_track[start].val;
			}
			else
			{
				const float dt = static_cast<float>(animated_bone.rotation_track[start + 1].time - animated_bone.rotation_track[start].time);
				const float factor = (time_in_ticks - animated_bone.rotation_track[start].time) / dt;
				assert(factor >= 0.0f && factor <= 1.0f);

				curr_pose.rotation = slerp(animated_bone.rotation_track[start].val, animated_bone.rotation_track[start + 1].val, factor);
				// rotation =  * rotation;
				curr_pose.rotation.normalize();
			}
		}
	}

}