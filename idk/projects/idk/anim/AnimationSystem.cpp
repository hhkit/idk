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
		if (_was_paused)
		{
			for (auto& elem : animators)
			{
				elem.Reset();
				elem.SaveBindPose();
				if (elem._curr_animation >= 0)
					elem.Play(elem._curr_animation);
			}
			_was_paused = false;
		}
		for (auto& elem : animators)
		{
			const auto anim = elem.GetAnimationRsc(elem._curr_animation);
			if (elem._is_playing && anim)
			{
				AnimationPass(elem);
			}
		}
		// Blend pass
		// Mix pass
	}

	void AnimationSystem::UpdatePaused(span<Animator> animators)
	{
		if (!_was_paused)
		{
			for (auto& elem : animators)
			{
				elem.Reset();
				elem.RestoreBindPose();
			}
			_was_paused = true;
		}
		for (auto& elem : animators)
		{
			const auto anim = elem.GetAnimationRsc(elem._curr_animation);
			if (elem._preview_playback && anim)
			{
				AnimationPass(elem);
			}
		}
	}

	void AnimationSystem::Shutdown()
	{
	}

	void AnimationSystem::AnimationPass(Animator& animator)
	{
		const auto anim_state = animator.GetAnimationState(animator._curr_animation);
		const auto anim = anim_state.animation;
		const auto& skeleton = animator._skeleton->data();

		if (animator._elapsed >= anim->GetDuration())
		{
			animator._elapsed -= anim->GetDuration();

			// Stop here if the animation does not loop
			if (!animator._is_looping)
			{
				animator._elapsed = 0.0f;
				animator._is_playing = false;
				return;
			}
		}

		const float ticks = animator._elapsed * anim->GetFPS() * anim_state.speed;
		// const float num_ticks = anim->GetNumTicks();
		// const float time_in_ticks = fmod(ticks, num_ticks);
		_blend = animator._blend_factor;

		// Loop through the skeleton
		for (size_t bone_id = 0; bone_id < skeleton.size(); ++bone_id)
		{
			const auto& curr_bone = skeleton[bone_id];
			const anim::AnimatedBone* animated_bone = anim->GetAnimatedBone(curr_bone._name);

			auto& curr_go = animator._child_objects[bone_id];
			if (!curr_go || animated_bone == nullptr)
				continue;

			matrix_decomposition<real> curr_pose = animator._bind_pose[bone_id];
			// Interpolate function will fill in the curr_pose as needed. If there are no keys, it will keep it in bind pose.
			InterpolateBone(*animated_bone, ticks, curr_pose);

			// During GenerateTransforms in the Animator, it will use the child transforms to 
			// generate the final transforms
			curr_go->Transform()->position = curr_pose.position;
			curr_go->Transform()->rotation = curr_pose.rotation;
			curr_go->Transform()->scale = curr_pose.scale;

		}

		// Check if the animator wants to stop after this update
		if (animator._is_stopping == true)
		{
			animator._is_playing = false;
			animator._preview_playback = false;
			animator._is_stopping = false;
			return;
		}

		// Compute the time
		animator._elapsed += Core::GetRealDT().count();
	
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
				IDK_ASSERT(factor >= 0.0f && factor <= 1.0f);

				curr_pose.scale = lerp(lerp(animated_bone.scale_track[start].val, animated_bone.scale_track[start + 1].val, factor), curr_pose.scale, _blend);
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
				IDK_ASSERT(factor >= 0.0f && factor <= 1.0f);

				curr_pose.position = lerp(lerp(animated_bone.translate_track[start].val, animated_bone.translate_track[start + 1].val, factor), curr_pose.position, _blend);
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
				IDK_ASSERT(factor >= 0.0f && factor <= 1.0f);

				curr_pose.rotation = slerp(slerp(animated_bone.rotation_track[start].val, animated_bone.rotation_track[start + 1].val, factor), curr_pose.rotation, _blend);
				curr_pose.rotation.normalize();
			}
		}
	}

}