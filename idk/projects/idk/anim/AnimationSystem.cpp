#include "stdafx.h"
#include "AnimationSystem.h"

#include <core/GameState.h>
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
		GameState::GetGameState().OnObjectCreate<Animator>() += [&](Handle<Animator> animator)
		{
			if (!animator)
				return;
			
			_creation_queue.push_back(animator);
		};
	}

	void AnimationSystem::Update(span<Animator> animators)
	{
		InitializeAnimators();
		if (_was_paused)
		{
			for (auto& elem : animators)
			{
				elem.Reset();
				SaveBindPose(elem);
				for (auto& layer : elem.layers)
				{
					if (layer.curr_state != string{})
						layer.Play(layer.curr_state);
				}
				
			}
			_was_paused = false;
		}
		for (auto& animator : animators)
		{
			// Update the layer logic first. 
			const size_t num_layers_playing = LayersPass(animator);

			// Don't bother doing the interpolation if there aren't any layers playing.
			if (num_layers_playing == 0)
			{
				FinalPass(animator);
				continue;
			}

			// Loop over all valid bones
			//const auto& skeleton = animator.skeleton->data();
			for (size_t child_index = 0; child_index < animator._child_objects.size(); ++child_index)
			{
				auto& child_go = animator._child_objects[child_index];
				if (!child_go)
					continue;

				// Pass this child through all the layers to get the final transformation of the bone
				// Find the last fully weighted bone layer
				size_t start_layer = 0;
				for (size_t k = 0; k < animator.layers.size(); ++k)
				{
					const auto& layer = animator.layers[k];
					if (layer.bone_mask[child_index] == true && abs(1.0f - layer.weight) < constants::epsilon<float>())
						start_layer = k;
				}

				// Initialize the final bone pose to the current layer
				BonePose final_bone_pose = AnimationPass(animator, animator.layers[start_layer], child_index);
				// From the start layer up, do the animation pass for each layer and blend
				for (size_t layer_index = start_layer + 1; layer_index < animator.layers.size(); ++layer_index)
				{
					// Blend all animations from this layer to the next based on the layer weight
					auto& curr_layer = animator.layers[layer_index];

					if (curr_layer.blend_type == AnimLayerBlend::Override_Blend)
					{
						// We do a blend from the curr layer to the final bone pose: 
						// If curr layer weight = 0.25, means we override 0.25 of the previous layer's animation.
						// This means we do Blend(curr_pose, layer_pose, 0.25);
						BonePose layer_pose = AnimationPass(animator, curr_layer, child_index);
						final_bone_pose = BlendPose(final_bone_pose, layer_pose, curr_layer.weight);
					}
				}

				auto child_xform = child_go->GetComponent<Transform>();
				child_xform->position = final_bone_pose.position;
				child_xform->scale = final_bone_pose.scale;
				child_xform->rotation = final_bone_pose.rotation;
			}// end for each bone
			AdvanceLayers(animator);
			FinalPass(animator);
		} // end for each animator
	}

	void AnimationSystem::UpdatePaused(span<Animator> animators)
	{
		InitializeAnimators();
		if (!_was_paused)
		{
			for (auto& elem : animators)
			{
				elem.Reset();
				RestoreBindPose(elem);
			}
			_was_paused = true;
		}
		for (auto& animator : animators)
		{
			if (!animator.preview_playback)
			{
				FinalPass(animator);
				continue;
			}

			// Update the layer logic first. 
			const size_t num_layers_playing = LayersPass(animator);

			// Don't bother doing the interpolation if there aren't any layers playing.
			if (num_layers_playing == 0)
			{
				FinalPass(animator);
				continue;
			}

			// Loop over all valid bones
			//const auto& skeleton = animator.skeleton->data();
			for (size_t child_index = 0; child_index < animator._child_objects.size(); ++child_index)
			{
				auto& child_go = animator._child_objects[child_index];
				if (!child_go)
					continue;

				// Pass this child through all the layers to get the final transformation of the bone
				// Find the last fully weighted bone layer
				size_t start_layer = 0;
				for (size_t k = 0; k < animator.layers.size(); ++k)
				{
					const auto& layer = animator.layers[k];
					if (layer.bone_mask[child_index] == true && abs(1.0f - layer.weight) < constants::epsilon<float>())
						start_layer = k;
				}

				// Initialize the final bone pose to the current layer
				BonePose final_bone_pose = AnimationPass(animator, animator.layers[start_layer], child_index);
				// From the start layer up, do the animation pass for each layer and blend
				for (size_t layer_index = start_layer + 1; layer_index < animator.layers.size(); ++layer_index)
				{
					// Blend all animations from this layer to the next based on the layer weight
					auto& curr_layer = animator.layers[layer_index];

					if (curr_layer.blend_type == AnimLayerBlend::Override_Blend)
					{
						// We do a blend from the curr layer to the final bone pose: 
						// If curr layer weight = 0.25, means we override 0.25 of the previous layer's animation.
						// This means we do Blend(curr_pose, layer_pose, 0.25);
						BonePose layer_pose = AnimationPass(animator, curr_layer, child_index);
						final_bone_pose = BlendPose(final_bone_pose, layer_pose, curr_layer.weight);
					}
				}

				auto child_xform = child_go->GetComponent<Transform>();
				child_xform->position = final_bone_pose.position;
				child_xform->scale = final_bone_pose.scale;
				child_xform->rotation = final_bone_pose.rotation;
				
			} // end for each bone
			AdvanceLayers(animator);
			FinalPass(animator);
		} // end for each animator
	}

	void AnimationSystem::Shutdown()
	{
	}

	AnimationSystem::BonePose AnimationSystem::AnimationPass(Animator& animator, AnimationLayer& layer, size_t bone_index)
	{
		BonePose result = animator._bind_pose[bone_index];
		// We don't care if the layer is playing or not. We only concern ourselves with the state logic
		// We return the bind pose if the layer isnt playing
		
		// Check if the state is enabled
		auto& anim_state = animator.GetAnimationState(layer.curr_state);
		if (!anim_state.enabled)
			return result;

		// We don't support blend trees yet
		if (anim_state.IsBlendTree())
			return result;

		auto& curr_go = animator._child_objects[bone_index];
		if (!curr_go)
			return result;

		// Get the actual animation data
		auto anim_data = anim_state.GetBasicState();

		// Compute the number of ticks this loop given the normalized time. 
		const float ticks = std::min(layer.normalized_time, 1.0f) * anim_data->motion->GetNumTicks();

		// The motion contains a hash table of all bones that are animated. If there isn't an animated bone, we get a nullptr
		const anim::AnimatedBone* animated_bone = anim_data->motion->GetAnimatedBone(curr_go->GetComponent<Bone>()->_bone_name);
		 if(animated_bone == nullptr)
			return result;

		// Interpolate from the found keyframe to the next keyframe and store the result.
		InterpolateBone(*animated_bone, ticks, result);
		return result;
	}

	AnimationSystem::BonePose AnimationSystem::BlendPose(const BonePose& from, const BonePose& to, float delta)
	{
		BonePose result;
		result.position = lerp(from.position, to.position, delta);
		result.rotation = slerp(from.rotation, to.rotation, delta);
		result.scale = lerp(from.scale, to.scale, delta);
		return result;

	}

	size_t AnimationSystem::LayersPass(Animator& animator)
	{
		// We assume that the animator is playing here.
		// Update the layer's logic. Handle advancing in time etc
		size_t num_playing = 0;
		for (auto& layer : animator.layers)
		{
			auto& anim_state = animator.GetAnimationState(layer.curr_state);
			if (!anim_state.enabled)
				continue;

			// We don't support blend trees yet
			if (anim_state.IsBlendTree())
				continue;

			if (!layer.is_playing)
				continue;

			if (layer.normalized_time >= 1.0f)
			{
				// Stop here if the animation does not loop
				// We dont subtract normalized_time because the designers might check
				// normalized_time >= 1.0f to check if an animation has ended
				if (!anim_state.loop)
				{
					//animator.layers[0].normalized_time = 0.0f;
					layer.is_playing = false;
					continue;
				}

				layer.normalized_time -= 1.0f;
			}

			// Check if the animator wants to stop after this update
			if (layer.is_stopping == true)
			{
				layer.is_playing = false;
				layer.is_stopping = false;
				continue;
			}
			
			++num_playing;
		}
		return num_playing;
	}

	void AnimationSystem::AdvanceLayers(Animator& animator)
	{
		// Adavance all layers by DT only if they are playing
		for (auto& layer : animator.layers)
		{
			if (!layer.is_playing)
				continue;

			auto& anim_state = animator.GetAnimationState(layer.curr_state);
			if (!anim_state.enabled)
				continue;

			// We don't support blend trees yet
			if (anim_state.IsBlendTree())
				continue;

			auto anim_data = anim_state.GetBasicState();

			layer.normalized_time += Core::GetRealDT().count() / anim_data->motion->GetDuration() * anim_state.speed;
		}
	}

	void AnimationSystem::FinalPass(Animator& animator)
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

	void AnimationSystem::InitializeAnimators()
	{
		// only remove from queue if the animator was created successfully
		if (_creation_queue.empty())
			return;

		// Call the build scene graph update
		Core::GetSystem<SceneManager>().BuildSceneGraph(GameState::GetGameState().GetObjectsOfType<const GameObject>());

		vector<Handle<Animator>> uncreated;
		uncreated.reserve(_creation_queue.size());

		for(auto& animator : _creation_queue)
		{
			const auto scene = Core::GetSystem<SceneManager>().GetSceneByBuildIndex(animator->GetHandle().scene);
			auto* sg = Core::GetSystem<SceneManager>().FetchSceneGraphFor(animator->GetGameObject());

			if (!sg)
			{
				uncreated.push_back(animator);
				continue;
			}

			if (animator->skeleton)
			{
				size_t num_bones = animator->skeleton->data().size();
				animator->_bind_pose.resize(num_bones);
				animator->_child_objects.resize(num_bones);
				animator->pre_global_transforms.resize(num_bones);
				animator->final_bone_transforms.resize(num_bones);
			}

			auto& child_objects = animator->_child_objects;
			const auto initialize_children =
				[&child_objects](Handle<GameObject> c_go, int)
			{
				auto c_bone = c_go->GetComponent<Bone>();
				if (c_bone)
				{
					child_objects[c_bone->_bone_index] = c_go;
				}
			};

			sg->visit(initialize_children);
			Core::GetSystem<AnimationSystem>().SaveBindPose(*animator);

			for (auto& layer : animator->layers)
			{
				layer.curr_state = layer.default_state;
				layer.weight = layer.default_weight;
			}
		}

		_creation_queue = std::move(uncreated);
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
		const auto scene = Core::GetSystem<SceneManager>().GetSceneByBuildIndex(animator.GetGameObject().scene);

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