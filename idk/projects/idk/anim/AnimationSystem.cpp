#include "stdafx.h"
#include "AnimationSystem.h"

#include <core/GameObject.h>
#include <common/Transform.h>
#include <anim/Bone.h>
#include <math/arith.h>
#include <math/matrix_transforms.h>
#include <iostream>

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
				for (auto& layer : elem.layers)
				{
					if (layer.curr_state != string{})
						elem.Play(layer.curr_state);
				}
				
			}
			_was_paused = false;
		}

		for (auto& elem : animators)
		{
			// TODO: Loop over layers here
			auto& anim_state = elem.GetAnimationState(elem.layers[0].curr_state);
			if (!anim_state.enabled)
				continue;

			// We don't support blend trees yet
			if (anim_state.IsBlendTree())
				continue;

			auto anim_data = anim_state.GetBasicState();
			if ((elem.layers[0].is_playing || elem.layers[0].is_stopping) && anim_data->motion)
			{
				AnimationPass(elem, elem.layers[0]);
				LayersPass(elem);
				FinalPass(elem, elem.layers[0]);
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
			// TODO: Loop over layers here
			for (auto& layer : elem.layers)
			{
				auto& anim_state = elem.GetAnimationState(layer.curr_state);
				if (!anim_state.enabled)
					continue;

				// We don't support blend trees yet
				if (anim_state.IsBlendTree())
					continue;

				auto anim_data = anim_state.GetBasicState();
				if (elem.preview_playback && anim_data->motion)
				{
					AnimationPass(elem, layer);
				}
				
			}
			if(elem.preview_playback)
				LayersPass(elem);
			FinalPass(elem, elem.layers[0]);
		}
	}

	void AnimationSystem::Shutdown()
	{
	}

	void AnimationSystem::AnimationPass(Animator& animator, AnimationLayer& layer)
	{
		auto anim_state = animator.GetAnimationState(layer.curr_state);
		auto& basic_state = *anim_state.GetBasicState();
		const auto& motion = basic_state.motion;
		const auto& skeleton = animator.skeleton->data();

		if (layer.normalized_time >= 1.0f)
		{
			layer.normalized_time -= 1.0f;

			// Stop here if the animation does not loop
			if (!anim_state.loop)
			{
				//animator.layers[0].normalized_time = 0.0f;
				layer.is_playing = false;
				return;
			}
		}
		
		const float ticks = layer.normalized_time * motion->GetNumTicks();
		// const float num_ticks = anim->GetNumTicks();
		// const float time_in_ticks = fmod(ticks, num_ticks);
		_blend = 0.0f;// animator.layers[0].blend_time;

		// Loop through the skeleton
		for (size_t bone_id = 0; bone_id < skeleton.size(); ++bone_id)
		{
			const auto& curr_bone = skeleton[bone_id];
			const anim::AnimatedBone* animated_bone = motion->GetAnimatedBone(curr_bone._name);

			auto& curr_go = animator._child_objects[bone_id];
			if (!curr_go || animated_bone == nullptr)
				continue;

			// Check if this bone has any more weights to distribute
			auto c_bone = curr_go->GetComponent<Bone>();
			if (c_bone->weight_left < 0.001f)
				continue;

			matrix_decomposition<real> curr_pose = animator._bind_pose[bone_id];
			// Interpolate function will fill in the curr_pose as needed. If there are no keys, it will keep it in bind pose.
			InterpolateBone(*animated_bone, ticks, curr_pose);

			// During GenerateTransforms in the Animator, it will use the child transforms to 
			// generate the final transforms
			layer.curr_poses[bone_id].position = curr_pose.position;
			layer.curr_poses[bone_id].rotation = curr_pose.rotation;
			layer.curr_poses[bone_id].scale = curr_pose.scale;
		}

		// Check if the animator wants to stop after this update
		if (layer.is_stopping == true)
		{
			layer.is_playing = false;
			layer.is_stopping = false;
			return;
		}

		// Compute the time
		layer.normalized_time += Core::GetRealDT().count() / motion->GetDuration() * anim_state.speed;
	}

	AnimationSystem::BonePose AnimationSystem::BlendPose(const BonePose& from, const BonePose& to, float delta)
	{
		BonePose result;
		result.position = lerp(from.position, to.position, delta);
		result.rotation = slerp(from.rotation, to.rotation, delta);
		result.scale = lerp(from.scale, to.scale, delta);
		return result;

	}

	void AnimationSystem::LayersPass(Animator& animator)
	{
		// TODO: Blend layer poses here
		// Start looping from the top. 
		// For a layer that has a weight of 1 and additive, we mark all bones affected by that layer to be done. 
		
		// MERGE STEP:
		// To merge two layers, we simply add the translations, multiply the rotations -> top to bottom
		// If it is additive, we simply add/multiply the poses.
		// If it is override, we check if the bone is done already or not.
		// Could this be done in ANimationPass? 
		

		for (size_t i = 0; i < animator._child_objects.size(); ++i)
		{
			auto& curr_go = animator._child_objects[i];
			if (!curr_go)
				continue;

			BonePose final_bone_pose;
			size_t start_layer = 0;

			// Find the last fully weighted bone
			for (size_t k = 0; k < animator.layers.size(); ++k)
			{
				const auto& layer = animator.layers[k];
				if (layer.bone_mask[i] == true && abs(1.0f - layer.weight) < constants::epsilon<float>())
					start_layer = k;
			}

			// We get the bone pose for the start layer and blend upwards
			final_bone_pose = animator.layers[start_layer].curr_poses[i];

			for (size_t k = start_layer + 1; k < animator.layers.size(); ++k)
			{
				// Blend all animations from this layer to the next based on the layer weight
				auto& curr_layer = animator.layers[k];
				if (curr_layer.blend_type == AnimLayerBlend::Override_Blend)
				{
					// We do a blend from the curr layer to the final bone pose: 
					// If curr layer weight = 0.25, means we override 0.25 of the previous layer's animation.
					// This means we do Blend(curr_pose, layer_pose, 0.25);
					const auto& layer_pose = curr_layer.curr_poses[i];
					final_bone_pose = BlendPose(final_bone_pose, layer_pose, curr_layer.weight);
				}
			}

			curr_go->Transform()->position	= final_bone_pose.position;
			curr_go->Transform()->rotation	= final_bone_pose.rotation;
			curr_go->Transform()->scale		= final_bone_pose.scale;
		}
		
	}

	void AnimationSystem::FinalPass(Animator& animator, AnimationLayer&)
	{
		// We grab the parent transform if its there, and multiply it with the child local.
		// We save this result into pre_transforms and then multiply that by global inverse bind pose 
		// and put it into final transforms.
		const auto global_inverse = animator.skeleton->GetGlobalInverse();
		const auto& skeleton = animator.skeleton->data();

		// Don't do anything if the sizes dont match.
		if (skeleton.size() != animator._child_objects.size())
		{
			std::cout << "[Animation System] Error: " << "Skeleton size of " << skeleton.size() << 
				" and game object hierarchy of size " << animator._child_objects.size() << " don't match.\n";
			animator.layers[0].is_playing = false;
			return;
		}
		

		if (global_inverse != mat4{})
		{
			for (size_t i = 0; i < animator._child_objects.size(); ++i)
			{
				auto& curr_bone = skeleton[i];
				auto& curr_go = animator._child_objects[i];

				if (!curr_go)
					continue;

				const auto parent_index = skeleton[i]._parent;
				if (parent_index >= 0)
				{
					const mat4& p_transform = animator.pre_global_transforms[parent_index];
					animator.pre_global_transforms[i] = p_transform * curr_go->Transform()->LocalMatrix();
				}
				else
				{
					animator.pre_global_transforms[i] = curr_go->Transform()->LocalMatrix();
				}

				animator.final_bone_transforms[i] = global_inverse * animator.pre_global_transforms[i] * curr_bone._global_inverse_bind_pose;

				curr_go->GetComponent<Bone>()->weight_left = 1.0f;
			}
		}
		else
		{
			for (size_t i = 0; i < animator._child_objects.size(); ++i)
			{
				auto& curr_bone = skeleton[i];
				auto& curr_go = animator._child_objects[i];

				if (!curr_go)
					continue;
				const auto parent_index = skeleton[i]._parent;
				if (parent_index >= 0)
				{
					// If we have the parent, we push in the parent.global * child.local
					const mat4& p_transform = animator.pre_global_transforms[parent_index];
					const mat4& c_transform = curr_go->Transform()->LocalMatrix();
					animator.pre_global_transforms[i] = p_transform * c_transform;
				}
				else
				{
					animator.pre_global_transforms[i] = curr_go->Transform()->LocalMatrix();
				}
				//const auto test = decompose(_pre_global_transforms[i]);
				animator.final_bone_transforms[i] = animator.pre_global_transforms[i] * curr_bone._global_inverse_bind_pose;
				curr_go->GetComponent<Bone>()->weight_left = 1.0f;
			}
		}
	}

	void AnimationSystem::InterpolateBone(const anim::AnimatedBone& animated_bone, float time_in_ticks, matrix_decomposition<real>& curr_pose)
	{
		const auto find_key = [&](const auto& vec, float ticks) -> size_t
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

	void AnimationSystem::GenerateSkeletonTree(Animator& animator)
	{
		if (!animator.skeleton)
			return;
		
		// We also need to generate all the game objects here.
		// The game object's transform is the inverse of bone_offset.
		const auto scene = Core::GetSystem<SceneManager>().GetSceneByBuildIndex(animator.GetHandle().scene);

		// Reset the animator
		HardReset(animator);

		// Resize all data
		const auto& bones = animator.skeleton->data();
		animator._bind_pose.resize(bones.size());
		animator._child_objects.resize(bones.size());
		animator.pre_global_transforms.resize(bones.size());
		animator.final_bone_transforms.resize(bones.size());
		
		// Resize all layers
		for (auto& layer : animator.layers)
			layer.curr_poses.resize(bones.size());

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
		if (!animator.skeleton)
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
			const auto parent_index = animator.skeleton->data()[i]._parent;
			if (parent_index >= 0)
			{
				// If we have the parent, we push in the parent.global * child.local
				const mat4& p_transform = animator.pre_global_transforms[parent_index];
				mat4 final_local_transform = p_transform * local_bind_pose_mat;
				animator.pre_global_transforms[i] = final_local_transform;
			}
			else
				animator.pre_global_transforms[i] = local_bind_pose_mat;
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
		animator.pre_global_transforms.clear();
		animator.final_bone_transforms.clear();
		animator._bind_pose.clear();
	}

}