#include "stdafx.h"
#include "AnimationSystem.h"
#include "AnimationUtils.h"

#include <core/GameState.h>
#include <core/GameObject.h>
#include <common/Transform.h>
#include <anim/Bone.h>
#include <math/arith.h>
#include <math/matrix_transforms.h>
#include <scene/SceneManager.h>

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
		InitializeAnimators(true);
		if (_was_paused)
		{
			for (auto& elem : animators)
			{
				elem.ResetToDefault();
				// SaveBindPose(elem);
				for (auto& layer : elem.layers)
				{
					if (layer.curr_state.index != 0)
						layer.Play(layer.curr_state.index);
				}

			}
			_was_paused = false;
		}
		for (auto& animator : animators)
		{
			if (!animator._initialized)
				continue;

			// Update the layer logic first. 
			const size_t num_layers_playing = LayersPass(animator);

			// Don't bother doing the interpolation if there aren't any layers playing.
			if (num_layers_playing == 0)
			{
				FinalPass(animator);
				continue;
			}

			// Loop over all valid bones
			const auto& skeleton = animator.skeleton->data();
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
					if (layer.bone_mask[child_index] && abs(1.0f - layer.weight) < constants::epsilon<float>())
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
				child_xform->rotation = skeleton[child_index].pre_rotation * final_bone_pose.rotation * skeleton[child_index].post_rotation;
			}// end for each bone
			AdvanceLayers(animator);
			FinalPass(animator);
		} // end for each animator
	}

	void AnimationSystem::UpdatePaused(span<Animator> animators)
	{
		InitializeAnimators(false);
		if (!_was_paused)
		{
			for (auto& elem : animators)
			{
				elem.ResetToDefault();
				RestoreBindPose(elem);
			}
			_was_paused = true;
		}
		for (auto& animator : animators)
		{
			if (!animator._initialized)
				continue;

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
			const auto& skeleton = animator.skeleton->data();
			for (size_t child_index = 0; child_index < animator._child_objects.size(); ++child_index)
			{
				auto& child_go = animator._child_objects[child_index];
				if (!child_go)
					continue;

				// Pass this child through all the layers to get the final transformation of the bone
				// Find the last fully weighted bone layer
				size_t start_layer = 0;
				for (size_t k = 1; k < animator.layers.size(); ++k)
				{
					const auto& layer = animator.layers[k];
					if (layer.bone_mask[child_index] && abs(1.0f - layer.weight) < constants::epsilon<float>())
						start_layer = k;
				}

				// Initialize the final bone pose to the current layer
				BonePose final_bone_pose = AnimationPass(animator, animator.layers[start_layer], child_index);

				// From the start layer up, do the animation pass for each layer and blend
				for (size_t layer_index = start_layer + 1; layer_index < animator.layers.size(); ++layer_index)
				{
					// Blend all animations from this layer to the next based on the layer weight
					auto& curr_layer = animator.layers[layer_index];
					if (curr_layer.bone_mask[child_index])
					{
						if (curr_layer.blend_type == AnimLayerBlend::Override_Blend)
						{
							// We do a blend from the curr layer to the final bone pose: 
							// If curr layer weight = 0.25, means we override 0.25 of the previous layer's animation.
							// This means we do Blend(curr_pose, layer_pose, 0.25);
							BonePose layer_pose = AnimationPass(animator, curr_layer, child_index);
							final_bone_pose = BlendPose(final_bone_pose, layer_pose, curr_layer.weight);
						}
					}
				}

				auto child_xform = child_go->GetComponent<Transform>();
				child_xform->position = final_bone_pose.position;
				child_xform->scale = final_bone_pose.scale;
				child_xform->rotation = skeleton[child_index].pre_rotation * final_bone_pose.rotation * skeleton[child_index].post_rotation;

			} // end for each bone
			AdvanceLayers(animator);
			FinalPass(animator);
		} // end for each animator
	}

	void AnimationSystem::Shutdown()
	{
	}

	AnimationSystem::BonePose AnimationSystem::ComputePose(Animator& animator, AnimationLayer& layer, AnimationLayerState& state, size_t bone_index)
	{
		UNREFERENCED_PARAMETER(layer);
		// Default result should be the bind pose
		BonePose result = animator._bind_pose[bone_index];
		result.rotation = quat{};

		// Check if the state is valid
		auto& anim_state = layer.GetAnimationState(state.index);
		if (!anim_state.valid)
		{
			state.is_stopping = true;
			return result;
		}

		auto& curr_go = animator._child_objects[bone_index];
		if (!curr_go)
			return result;

		if (anim_state.IsBlendTree())
			return ComputeBlendTreePose(animator, layer, state, bone_index);

		// Get the actual animation data
		auto anim_data = anim_state.GetBasicState();

		if (!anim_data->motion)
		{
			state.is_stopping = true;
			return result;
		}

		// Compute the number of ticks this loop given the normalized time. 
		const float ticks = std::min(state.normalized_time, 1.0f) * anim_data->motion->GetNumTicks();

		// The motion contains a hash table of all bones that are animated. If there isn't an animated bone, we get a nullptr
		const anim::AnimatedBone* animated_bone = anim_data->motion->GetAnimatedBone(curr_go->GetComponent<Bone>()->bone_name);
		if (animated_bone == nullptr)
			return result;

		// Interpolate from the found keyframe to the next keyframe and store the result.
		InterpolateBone(*animated_bone, ticks, result);

		return result;
	}

	AnimationSystem::BonePose AnimationSystem::ComputeBlendTreePose(Animator& animator, AnimationLayer& layer, AnimationLayerState& state, size_t bone_index)
	{
		UNREFERENCED_PARAMETER(layer);
		// Default result should be the bind pose
		BonePose result = animator._bind_pose[bone_index];
		result.rotation = quat{};

		// Check if the state is valid
		auto& anim_state = layer.GetAnimationState(state.index);
		if (!anim_state.valid)
		{
			state.is_stopping = true;
			return result;
		}

		auto& curr_go = animator._child_objects[bone_index];
		if (!curr_go)
			return result;

		// Get the actual animation data
		auto anim_data = anim_state.GetBlendTree();

		// We dont supp more than 1D blend trees yet
		if (anim_data->blend_tree_type != anim::BlendTreeType::BlendTree_1D)
			return result;

		// First compute the weights
		if (!anim_data->weights_cached)
		{
			float param_val = anim_data->def_data[0];
			auto res = animator.GetParam<anim::FloatParam>(anim_data->params[0]);
			if (res.valid)
				param_val = res.val;

			anim_data->ComputeWeights(param_val);
			anim_data->weights_cached = true;
		}
		
		bool first_applied = false;
		// Interpolate each of the animation motions based on the weights
		auto& motions = anim_data->motions;
		for (auto& blend_motion : motions)
		{
			// Ignore all blend motions without valid clip
			if (!blend_motion.motion)
			{
				continue;
			}

			// Compute the number of ticks this loop given the normalized time. 
			const float ticks = std::min(state.normalized_time, 1.0f) * blend_motion.motion->GetNumTicks();

			BonePose curr_pose = animator._bind_pose[bone_index];
			curr_pose.rotation = quat{};

			// The motion contains a hash table of all bones that are animated. If there isn't an animated bone, we get a nullptr
			const anim::AnimatedBone* animated_bone = blend_motion.motion->GetAnimatedBone(curr_go->GetComponent<Bone>()->bone_name);
			if (animated_bone != nullptr)
			{
				// Interpolate from the found keyframe to the next keyframe and store the result.
				InterpolateBone(*animated_bone, ticks, curr_pose);
			}

			// If the a bone pose was applied to result alr, we want to add to the result and not just blend FROM the result
			if (first_applied)
			{
				// Apply additive animation to the result
				result.position += curr_pose.position * blend_motion.weight;
				result.rotation = slerp(quat{}, curr_pose.rotation, blend_motion.weight) * result.rotation;
				result.scale	+= curr_pose.scale * blend_motion.weight;
			}
			else
			{
				result.position = curr_pose.position * blend_motion.weight;
				result.rotation = slerp(quat{}, curr_pose.rotation, blend_motion.weight);
				result.scale	= curr_pose.scale * blend_motion.weight;
			}
			first_applied = true;
		}

		result.rotation.normalize();
		return result;
	}

	AnimationSystem::BonePose AnimationSystem::AnimationPass(Animator& animator, AnimationLayer& layer, size_t bone_index)
	{
		// If a layer is not playing, we return the previous pose it was in 
		const auto curr_index = layer.curr_state.index;
		const auto blend_index = layer.blend_state.index;

		if (!layer.IsPlaying())
		{
			return layer.prev_poses[bone_index];
		}
		
		BonePose result = animator._bind_pose[bone_index];
		result.rotation = quat{};
		if (!layer.bone_mask[bone_index])
		{
			return result;
		}

		if (layer.curr_state.is_playing && !layer.blend_interrupt)
		{
			result = ComputePose(animator, layer, layer.curr_state, bone_index);
			// If after ComputePose curr_state is now not playing, we know that something went wrong.
			if (layer.curr_state.is_stopping)
			{
				layer.curr_state.normalized_time = 0.0f;
				LOG_TO(LogPool::ANIM, "[Animator] Current animation (" + serialize_text(curr_index) + ") in layer (" + layer.name + ") doesn't exist.");
			}

			layer.blend_source[bone_index] = result;
		}
		
		if (layer.blend_state.is_playing)
		{
			BonePose blend_state_result = ComputePose(animator, layer, layer.blend_state, bone_index);
			// If after ComputePose curr_state now wants to stop, we know that something went wrong.
			if (layer.blend_state.is_stopping)
			{
				// layer.is_blending = false;
				layer.blend_state.normalized_time = 0.0f;
				layer.blend_state.elapsed_time = 0.0f;
				layer.blend_interrupt = false;
				layer.transition_index = 0;
				LOG_TO(LogPool::ANIM, "[Animator] Target blend animation (" + serialize_text(blend_index) + ") in layer (" + layer.name + ") doesn't exist.");
			}
			else
			{
				IDK_ASSERT(abs(layer.blend_duration) > 0.0001);
				const float blend_weight = layer.blend_state.elapsed_time / layer.blend_duration;
				result = BlendPose(layer.blend_source[bone_index], blend_state_result, blend_weight);
			}
		}

		layer.prev_poses[bone_index] = result;
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

	void AnimationSystem::EvaluateTransitions(Animator& animator, AnimationLayer& layer)
	{
		auto& curr_state = layer.curr_state;
		// Only evaluate curr state if it is playing
		
		if (curr_state.is_playing)
		{
			auto& anim_state = layer.GetAnimationState(curr_state.index);
			if (!anim_state.valid)
				return;

			// if we are in a transition and cannot be interrupted, we do not evaluate conditions/exit time
			if (layer.IsInTransition() && !layer.IsTransitionInterruptible())
			{
				return;
			}
				
			
			for (size_t i = 1; i < anim_state.transitions.size(); ++i)
			{
				// Skip invalid transitions
				auto& curr_transition = anim_state.GetTransition(i);
				// dont bother evaluating transition if the same transition is alr happening
				if (!curr_transition.valid || i == layer.transition_index)
					continue;

				
				// Evaluate exit time first
				if (curr_transition.has_exit_time)
				{
					bool exit_time_reached = false;
					if (curr_transition.exit_time <= 1.0f)
						exit_time_reached = curr_state.normalized_time >= curr_transition.exit_time;
					else
						exit_time_reached = curr_state.elapsed_time >= curr_transition.exit_time;

					// Do not transit if the exit time has not been reached (early out)
					if (exit_time_reached == false)
						continue;
				}
				
				bool transit = true;
				for (auto& cond : curr_transition.conditions)
				{
					switch (cond.type)
					{
					case anim::AnimDataType::INT:
					{
						auto& param = animator.GetParam<anim::IntParam>(cond.param_name);
						transit &= param.valid ? anim::condition_ops_int[cond.op_index](param.val, cond.val_i) : false;
						break;
					}
					case anim::AnimDataType::FLOAT:
					{
						auto& param = animator.GetParam<anim::FloatParam>(cond.param_name);
						transit &= param.valid ? anim::condition_ops_float[cond.op_index](param.val, cond.val_f) : false;
						break;
					}
					case anim::AnimDataType::BOOL:
					{
						auto& param = animator.GetParam<anim::BoolParam>(cond.param_name);
						transit &= param.valid ? param.val == cond.val_b : false;
						break;
					}
					case anim::AnimDataType::TRIGGER:
					{
						auto& param = animator.GetParam<anim::TriggerParam>(cond.param_name);
						transit &= param.valid ? param.val : false;
						break;
					}
					case anim::AnimDataType::NONE:
					{
						break;
					}
					} // end switch

					if (transit == false)
						break;
				}

				if (transit == true)
				{
					const bool blend_to_succeeded = layer.BlendTo(curr_transition.transition_to_index, curr_transition.transition_duration);
					if (blend_to_succeeded && layer.blend_state.is_playing)
					{
						layer.blend_state.normalized_time = curr_transition.transition_offset;
						layer.transition_index = i;
					}
					break;
				}
			}
		}
	}

	size_t AnimationSystem::LayersPass(Animator& animator)
	{
		// We assume that the animator is playing here.
		// Update the layer's logic. Handle advancing in time etc
		size_t num_playing = 0;
		for (auto& layer : animator.layers)
		{
			if (!layer.IsPlaying())
				continue;


			// Evaluate transitions before anything happens
			EvaluateTransitions(animator, layer);

			// Check both the blend state and the curr state
			if (layer.curr_state.normalized_time >= 1.0f)
			{
				auto& anim_state = layer.GetAnimationState(layer.curr_state.index);
				if (!anim_state.valid)
					continue;

				// Stop here if the animation does not loop
				// We dont subtract normalized_time because the designers might check
				// normalized_time >= 1.0f to check if an animation has ended
				if (!anim_state.loop)
				{
					layer.curr_state.is_playing = false;
					layer.curr_state.normalized_time = 1.0f;
				}
				else
					layer.curr_state.normalized_time -= 1.0f;
			}

			if (layer.blend_state.is_playing)
			{
				// Note: Blend states always loop
				if (layer.blend_state.normalized_time >= 1.0f)
				{
					auto& anim_state = layer.GetAnimationState(layer.blend_state.index);
					if (!anim_state.valid)
						continue;

					layer.blend_state.normalized_time -= 1.0f;
				}

				// Check if the blending is over
				float delta = layer.blend_state.elapsed_time / layer.blend_duration;
				if (delta >= 1.0f)
				{
					LOG("RESET HIT");
					layer.curr_state = layer.blend_state;
					layer.ResetBlend();
				}
			}

			// Check if the animator wants to stop after this update
			if (layer.curr_state.is_stopping == true)
			{
				const size_t curr_index = layer.curr_state.index;
				layer.curr_state.Reset();
				layer.curr_state.index = curr_index;
				/*layer.curr_state.normalized_time = 0.0f;
				layer.curr_state.is_playing = false;
				layer.curr_state.is_stopping = false;*/
			}

			if (layer.blend_state.is_stopping == true)
			{
				layer.ResetBlend();
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
			// Check both the blend state and the curr state
			if (layer.curr_state.is_playing)
			{
				auto& anim_state = layer.GetAnimationState(layer.curr_state.index);
				if (!anim_state.valid)
					continue;

				// We don't support blend trees yet
				if (anim_state.IsBlendTree())
					AdvanceBlendTree(layer.curr_state, anim_state);
				else
				{
					auto anim_data = anim_state.GetBasicState();
					const float inc = Core::GetRealDT().count() / anim_data->motion->GetDuration() * anim_state.speed;
					layer.curr_state.normalized_time += inc;
					layer.curr_state.elapsed_time += inc;
				}
				
			}

			if (layer.blend_state.is_playing)
			{
				auto& anim_state = layer.GetAnimationState(layer.blend_state.index);
				if (!anim_state.valid)
					continue;

				// We don't support blend trees yet
				if (anim_state.IsBlendTree())
					AdvanceBlendTree(layer.blend_state, anim_state);
				else
				{
					auto anim_data = anim_state.GetBasicState();
					const float inc = Core::GetRealDT().count() / anim_data->motion->GetDuration() * anim_state.speed;
					layer.blend_state.normalized_time += inc;
					layer.blend_state.elapsed_time += inc;
				}
			}

			// Always set this to false no matter what. This is a trigger so it should only be true for one frame.
			layer.blend_this_frame = false;
		}
	}

	void AnimationSystem::AdvanceBlendTree(AnimationLayerState& layer_state, AnimationState& anim_state) const
	{
		auto anim_data = anim_state.GetBlendTree();
		float time_inc = 0.0f;
		anim_data->weights_cached = false;
		const float dt = Core::GetRealDT().count();
		for (auto& blendtree_motion : anim_data->motions)
		{
			const float curr_inc = dt / blendtree_motion.motion->GetDuration() * blendtree_motion.speed;
			time_inc += curr_inc * blendtree_motion.weight;
		}

		layer_state.elapsed_time += time_inc;
		layer_state.normalized_time += time_inc;
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
			LOG_WARNING_TO(LogPool::ANIM, "[Animation System] Error: Skeleton size of " + std::to_string(skeleton.size()) +
				" and game object hierarchy of size " + std::to_string(animator._child_objects.size()) + " don't match.");

			// for (auto& layer : animator.layers)
			// {
			// 	layer.curr_state.is_playing = false;
			// 	layer.blend_state = AnimationLayerState{};
			// }
			
			// animator.layers[0].is_playing = false;
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

				const auto parent_index = skeleton[i].parent;
				if (parent_index >= 0)
				{
					const mat4& p_transform = animator.pre_global_transforms[parent_index];
					animator.pre_global_transforms[i] = p_transform * curr_go->Transform()->LocalMatrix();
				}
				else
				{
					animator.pre_global_transforms[i] = curr_go->Transform()->LocalMatrix();
				}

				animator.final_bone_transforms[i] = global_inverse * animator.pre_global_transforms[i] * curr_bone.global_inverse_bind_pose;

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
				const auto parent_index = skeleton[i].parent;
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
				animator.final_bone_transforms[i] = animator.pre_global_transforms[i] * curr_bone.global_inverse_bind_pose;
			}
		}

		animator.ResetTriggers();
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
				IDK_ASSERT(factor >= 0.0f && factor <= 1.0f);

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
				IDK_ASSERT(factor >= 0.0f && factor <= 1.0f);

				curr_pose.rotation = slerp(animated_bone.rotation_track[start].val, animated_bone.rotation_track[start + 1].val, factor);
				curr_pose.rotation.normalize();
			}
		}
	}

	void AnimationSystem::InitializeAnimators(bool play)
	{
		// only remove from queue if the animator was created successfully
		if (_creation_queue.empty())
			return;

		// Call the build scene graph update
		Core::GetSystem<SceneManager>().BuildSceneGraph(GameState::GetGameState().GetObjectsOfType<const GameObject>());

		vector<Handle<Animator>> uncreated;
		uncreated.reserve(_creation_queue.size());
		const auto add_to_uncreated = [&](auto& animator)
		{
			++animator->_intialize_count;
			// Only try initializing 60 frames
			if (animator->_intialize_count <= 60)
				uncreated.push_back(animator);
		};
		for(auto& animator : _creation_queue)
		{
			const auto scene = Core::GetSystem<SceneManager>().GetSceneByBuildIndex(animator->GetHandle().scene);
			auto* sg = Core::GetSystem<SceneManager>().FetchSceneGraphFor(animator->GetGameObject());

			if (!sg)
			{
				add_to_uncreated(animator);
				continue;
			}

			auto& child_objects = animator->_child_objects;
			const auto initialize_children =
				[&child_objects](Handle<GameObject> c_go, int)
			{
				auto c_bone = c_go->GetComponent<Bone>();
				if (c_bone)
				{
					const auto index = c_bone->bone_index;
					if(index >= 0)
						child_objects[index] = c_go;
				}
			};

			if (animator->skeleton)
			{
				size_t num_bones = animator->skeleton->data().size();
				animator->_bind_pose.resize(num_bones);
				animator->_child_objects.resize(num_bones);
				animator->pre_global_transforms.resize(num_bones);
				animator->final_bone_transforms.resize(num_bones);

				// Init child objects
				sg->visit(initialize_children);

				// Init bone component's children vector
				auto& skeleton_data = animator->skeleton->data();
				for (size_t i = 0; i < skeleton_data.size(); ++i)
				{
					auto& bone = skeleton_data[i];
					if (bone.parent >= 0)
					{
						// Put the current index into my parent's children vector.
						child_objects[bone.parent]->GetComponent<Bone>()->children.emplace_back(s_cast<uint8_t>(i));
					}
				}

				// Save the bind pose here
				Core::GetSystem<AnimationSystem>().SaveBindPose(*animator);

				for (auto& layer : animator->layers)
				{
					layer.prev_poses.resize(animator->skeleton->data().size());
					layer.blend_source.resize(animator->skeleton->data().size());
					if (layer.bone_mask.size() != animator->skeleton->data().size())
					{
						layer.bone_mask.resize(animator->skeleton->data().size(), 1);
					}
				}

				animator->ResetToDefault();
				if (play)
					for (auto& layer : animator->layers)
						layer.Play("");

				animator->_initialized = true;
			}
			else 
			{
				add_to_uncreated(animator);
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
			// auto transform = curr_bone.global_inverse_bind_pose.inverse();

			// mat4 local_bind_pose = curr_bone.local_bind_pose.recompose();
			obj->Name(curr_bone.name);
			obj->GetComponent<Transform>()->position = curr_bone.local_bind_pose.position;
			obj->GetComponent<Transform>()->rotation = curr_bone.local_bind_pose.rotation;
			obj->GetComponent<Transform>()->scale = curr_bone.local_bind_pose.scale;

			auto c_bone = obj->AddComponent<Bone>();
			c_bone->bone_name = curr_bone.name;
			c_bone->bone_index = s_cast<int>(i);

			if (curr_bone.parent >= 0)
				obj->GetComponent<Transform>()->SetParent(animator._child_objects[curr_bone.parent], false);
			else
				obj->GetComponent<Transform>()->SetParent(animator.GetGameObject(), false);

			animator._bind_pose[i] = curr_bone.local_bind_pose;
			animator._child_objects[i] = obj;
		}

		for (auto& layer : animator.layers)
		{
			layer.prev_poses.resize(animator.skeleton->data().size());
			layer.blend_source.resize(animator.skeleton->data().size());
		}
	}

	void AnimationSystem::SaveBindPose(Animator& animator)
	{
		// Need to save the local transforms of the child objects as bind pose
		for (size_t i = 0; i < animator._child_objects.size(); ++i)
		{
			auto curr_go = animator._child_objects[i];
			if (curr_go)
			{
				animator._bind_pose[i].position = curr_go->Transform()->position;
				animator._bind_pose[i].rotation = curr_go->Transform()->rotation;
				animator._bind_pose[i].scale = curr_go->Transform()->scale;
			}
		}
	}

	void AnimationSystem::RestoreBindPose(Animator& animator)
	{
		if (!animator.skeleton)
			return;

		// Need to revert back to the bind pose
		const auto& bones = animator.skeleton->data();
		for (size_t i = 0; i < animator._child_objects.size(); ++i)
		{
			if (!animator._child_objects[i])
				continue;

			auto local_bind_pose = bones[i].local_bind_pose;
			auto local_bind_pose_mat = local_bind_pose.recompose();

			animator._child_objects[i]->Transform()->position = local_bind_pose.position;
			animator._child_objects[i]->Transform()->rotation = local_bind_pose.rotation;
			animator._child_objects[i]->Transform()->scale = local_bind_pose.scale;

			// compute the bone_transform for the bind pose
			const auto parent_index = animator.skeleton->data()[i].parent;
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