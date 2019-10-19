#include "stdafx.h"
#include "AnimationSystem.h"

#include <core/GameObject.h>
#include <common/Transform.h>
#include <anim/Bone.h>
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
				SaveBindPose(elem);
				if (elem._curr_animation >= 0)
					elem.Play(elem._curr_animation);
			}
			_was_paused = false;
		}

		for (auto& elem : animators)
		{
			const auto anim = elem.GetAnimationRsc(elem._curr_animation);
			if ((elem._is_playing || elem._is_stopping) && anim)
			{
				AnimationPass(elem);
				FinalPass(elem);
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
				RestoreBindPose(elem);
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

			FinalPass(elem);
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

	void AnimationSystem::FinalPass(Animator& animators)
	{
		// We grab the parent transform if its there, and multiply it with the child local.
		// We save this result into pre_transforms and then multiply that by global inverse bind pose 
		// and put it into final transforms.
		const auto global_inverse = animators._skeleton->GetGlobalInverse();
		const auto& skeleton = animators._skeleton->data();
		if (global_inverse != mat4{})
		{
			for (size_t i = 0; i < animators._child_objects.size(); ++i)
			{
				auto& curr_bone = skeleton[i];
				auto& curr_go = animators._child_objects[i];

				if (!curr_go)
					continue;
				const auto parent_index = skeleton[i]._parent;
				if (parent_index >= 0)
				{
					const mat4& p_transform = animators._pre_global_transforms[parent_index];
					animators._pre_global_transforms[i] = p_transform * curr_go->Transform()->LocalMatrix();
				}
				else
				{
					animators._pre_global_transforms[i] = curr_go->Transform()->LocalMatrix();
				}

				animators._final_bone_transforms[i] = global_inverse * animators._pre_global_transforms[i] * curr_bone._global_inverse_bind_pose;
			}
		}
		else
		{
			for (size_t i = 0; i < animators._child_objects.size(); ++i)
			{
				auto& curr_bone = skeleton[i];
				auto& curr_go = animators._child_objects[i];

				if (!curr_go)
					continue;
				const auto parent_index = skeleton[i]._parent;
				if (parent_index >= 0)
				{
					// If we have the parent, we push in the parent.global * child.local
					const mat4& p_transform = animators._pre_global_transforms[parent_index];
					const mat4& c_transform = curr_go->Transform()->LocalMatrix();
					animators._pre_global_transforms[i] = p_transform * c_transform;
				}
				else
				{
					animators._pre_global_transforms[i] = curr_go->Transform()->LocalMatrix();
				}
				//const auto test = decompose(_pre_global_transforms[i]);
				animators._final_bone_transforms[i] = animators._pre_global_transforms[i] * curr_bone._global_inverse_bind_pose;
			}
		}
	}

	void AnimationSystem::InterpolateBone(const anim::AnimatedBone& animated_bone, float time_in_ticks, matrix_decomposition<real>& curr_pose)
	{
		const auto test = [&](const auto& vec, float ticks) -> size_t
		{
			for (unsigned i = 0; i < vec.size(); ++i)
			{
				if (ticks < static_cast<float>(vec[i].time))
				{
					return i - 1;
				}
			}

			return vec.size() - 1;
		};

		// Scaling
		if (animated_bone.scale_track.size() > 1)
		{
			const size_t start = test(animated_bone.scale_track, time_in_ticks);
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
			const size_t start = test(animated_bone.translate_track, time_in_ticks);
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
			const size_t start = test(animated_bone.rotation_track, time_in_ticks);
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

	void AnimationSystem::GenerateSkeletonTree(Animator& animator)
	{
		if (!animator._skeleton)
			return;
		
		// We also need to generate all the game objects here.
		// The game object's transform is the inverse of bone_offset.
		const auto scene = Core::GetSystem<SceneManager>().GetSceneByBuildIndex(animator.GetHandle().scene);

		// Reset the animator
		HardReset(animator);

		// Resize all data
		const auto& bones = animator._skeleton->data();
		animator._bind_pose.resize(bones.size());
		animator._child_objects.resize(bones.size());
		animator._pre_global_transforms.resize(bones.size());
		animator._final_bone_transforms.resize(bones.size());

		for (size_t i = 0; i < bones.size(); ++i)
		{
			auto& curr_bone = bones[i];

			auto obj = scene->CreateGameObject();
			// auto transform = curr_bone._global_inverse_bind_pose.inverse();

			// mat4 local_bind_pose = curr_bone._local_bind_pose.recompose();
			obj->Name(curr_bone._name);
			obj->GetComponent<Transform>()->position = curr_bone._local_bind_pose.position;
			obj->GetComponent<Transform>()->rotation = curr_bone._local_bind_pose.rotation;
			obj->GetComponent<Transform>()->scale = curr_bone._local_bind_pose.scale;

			auto c_bone = obj->AddComponent<Bone>();
			c_bone->_bone_name = curr_bone._name;
			c_bone->_bone_index = s_cast<int>(i);

			if (curr_bone._parent >= 0)
				obj->GetComponent<Transform>()->SetParent(animator._child_objects[curr_bone._parent], false);
			else
				obj->GetComponent<Transform>()->SetParent(animator.GetGameObject(), false);

			animator._bind_pose[i] = curr_bone._local_bind_pose;
			animator._child_objects[i] = obj;
		}
	}

	void AnimationSystem::SaveBindPose(Animator& animator)
	{
		// Need to save the local transforms of the child objects as bind pose
		for (size_t i = 0; i < animator._child_objects.size(); ++i)
		{
			auto curr_go = animator._child_objects[i];
			animator._bind_pose[i].position = curr_go->Transform()->position;
			animator._bind_pose[i].rotation = curr_go->Transform()->rotation;
			animator._bind_pose[i].scale = curr_go->Transform()->scale;
		}
	}

	void AnimationSystem::RestoreBindPose(Animator& animator)
	{
		if (!animator._skeleton)
			return;

		// Need to revert back to the bind pose
		// const auto& bones = _skeleton->data();
		for (size_t i = 0; i < animator._bind_pose.size(); ++i)
		{
			auto local_bind_pose = animator._bind_pose[i];
			auto local_bind_pose_mat = local_bind_pose.recompose();

			animator._child_objects[i]->Transform()->position = local_bind_pose.position;
			animator._child_objects[i]->Transform()->rotation = local_bind_pose.rotation;
			animator._child_objects[i]->Transform()->scale = local_bind_pose.scale;

			// compute the bone_transform for the bind pose
			const auto parent_index = animator._skeleton->data()[i]._parent;
			if (parent_index >= 0)
			{
				// If we have the parent, we push in the parent.global * child.local
				const mat4& p_transform = animator._pre_global_transforms[parent_index];
				mat4 final_local_transform = p_transform * local_bind_pose_mat;
				animator._pre_global_transforms[i] = final_local_transform;
			}
			else
				animator._pre_global_transforms[i] = local_bind_pose_mat;
		}
	}

	void AnimationSystem::HardReset(Animator& animator)
	{
		const auto scene = Core::GetSystem<SceneManager>().GetSceneByBuildIndex(animator.GetHandle().scene);

		for (auto& obj : animator._child_objects)
		{
			scene->DestroyGameObject(obj);
		}

		animator._child_objects.clear();
		animator._pre_global_transforms.clear();
		animator._final_bone_transforms.clear();
		animator._bind_pose.clear();
	}

}