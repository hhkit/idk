#include "stdafx.h" 
#include <iostream>
#include <idk.h>
#include <anim/Bone.h>
#include "common/Transform.h"
#include "core/GameObject.inl"
#include "Animator.inl"
#include <math/arith.h>
#include "scene/SceneManager.h"
#include "AnimationSystem.h"
#include <serialize/text.inl>
#include <res/ResourceHandle.inl>
#include "math/matrix_decomposition.inl"

namespace idk 
{
	Animator::Animator()
	{
		// Initialize the base layer. Cannot be removed or edited (much).
		AnimationLayer base_layer;
		base_layer.name = "Base Layer";
		std::fill(base_layer.bone_mask.begin(), base_layer.bone_mask.end(), true);
		
		// layer_table.emplace("Base Layer", 0);
		layers.push_back(base_layer);
	}

#pragma region Engine Getters/Setters

	const vector<mat4>& Animator::BoneTransforms()const
	{
		return final_bone_transforms;
	}

	void Animator::AddAnimation(RscHandle<anim::Animation> anim_rsc)
	{
		layers[0].AddAnimation(anim_rsc);
	}

	bool Animator::RenameAnimation(string_view from, string_view to)
	{
		return layers[0].RenameAnimation(from, to);
	}

	void Animator::RemoveAnimation(string_view name)
	{
		layers[0].RemoveAnimation(name);
	}

	void Animator::AddLayer()
	{
		string name = "New Layer";
		string append = "";
		// Check if name exists
		int count = 0;
		while (FindLayerIndex(name + append) < layers.size())
		{
			// Generate a unique name
			append = " " + std::to_string(count++);
		}

		AnimationLayer new_layer{};
		new_layer.name = name + append;
		new_layer.prev_poses.resize(skeleton->data().size());
		new_layer.blend_source.resize(skeleton->data().size());

		// All bones are initialized to be unmasked the start
		new_layer.bone_mask.resize(skeleton->data().size(), 1);
		
		layers.push_back(new_layer);
	}

	size_t Animator::FindLayerIndex(string_view name)
	{
		size_t index = 0;
		for (index = 0; index < layers.size(); ++index)
		{
			if (layers[index].name == name)
				break;
		}
		return index;
	}

	bool Animator::RenameLayer(string_view from, string_view to)
	{
		string from_str{ from };
		string to_str{ to };
		auto found_src = FindLayerIndex(from_str);
		if (found_src >= layers.size())
		{
			LOG_TO(LogPool::ANIM, string{ "Cannot rename animation layer (" } +from.data() + ") to (" + to.data() + ").");
			return false;
		}

		auto found_dest = FindLayerIndex(to_str);
		if (found_dest < layers.size())
		{
			LOG_TO(LogPool::ANIM, string{ "Cannot rename animation layer (" } +from.data() + ") to (" + to.data() + ").");
			return false;
		}

		layers[found_src].name = to_str;

		return true;
	}

	bool Animator::RemoveLayer(string_view name)
	{
		auto res = FindLayerIndex(name);
		return RemoveLayer(res);
	}

	bool Animator::RemoveLayer(size_t index)
	{
		if(index >= layers.size())
		{
			LOG_TO(LogPool::ANIM, string{ "Cannot delete animation layer (" } + serialize_text(index) + ".");
			return false;
		}

		layers.erase(layers.begin() + index);
		return true;
	}

#pragma endregion

#pragma region Editor Functionality
	void Animator::Reset()
	{
		preview_playback = false;
		for(auto& layer : layers)
			layer.Reset();

		for (auto& p : parameters.int_vars)
			p.second.ResetToDefault();

		for (auto& p : parameters.float_vars)
			p.second.ResetToDefault();

		for (auto& p : parameters.bool_vars)
			p.second.ResetToDefault();

		for (auto& p : parameters.trigger_vars)
			p.second.ResetToDefault();
	}

	void Animator::ResetToDefault()
	{
		for (size_t i = 0; i < layers.size(); ++i)
		{
			layers[i].ResetToDefault();
		}

		for (auto& p : parameters.int_vars)
			p.second.ResetToDefault();

		for (auto& p : parameters.float_vars)
			p.second.ResetToDefault();

		for (auto& p : parameters.bool_vars)
			p.second.ResetToDefault();

		for (auto& p : parameters.trigger_vars)
			p.second.ResetToDefault();
	}

	void Animator::OnPreview()
	{
		if (preview_playback)
		{
			for (auto& layer : layers)
			{
				auto& anim_state = layer.GetAnimationState(layer.curr_state.index);
				if (anim_state.valid)
				{

					bool has_valid_clip = true;
					const auto state = anim_state.GetBasicState();
					if (state)
					{
						has_valid_clip = s_cast<bool>(state->motion);
					}
					else
					{
						// All motions need to be valid for a blend tree to be valid
						for (auto& m : anim_state.GetBlendTree()->motions)
							if (!m.motion)
							{
								has_valid_clip = false;
								break;
							}
					}
					if (has_valid_clip)
						layer.Play(layer.curr_state.index);
				}
			}

			layers[0].weight = 1.0f;
		}
		else
		{
			ResetToDefault();
			Core::GetSystem<AnimationSystem>().RestoreBindPose(*this);
		}
	}

#pragma endregion

#pragma region Script Functions
	bool Animator::Play(string_view animation_name, float offset, int layer_index)
	{
		if (layer_index >= layers.size())
			return false;
		return layers[layer_index].Play(animation_name, offset);
	}

	bool Animator::BlendTo(string_view animation_name, float time, int layer_index)
	{
		if (layer_index >= layers.size())
			return false;

		// Cap blend duration to 1.0f
		time = std::min(abs(time), 1.0f);

		// If time is 0.0, we just call play cos its the same.
		if(time < 0.00001f)
			return layers[layer_index].Play(animation_name);
		else
			return layers[layer_index].BlendTo(animation_name, time);
	}

	bool Animator::Resume(int layer_index)
	{
		if (layer_index >= layers.size())
			return false;
		layers[layer_index].Resume();
		return true;
	}

	bool Animator::Pause(int layer_index)
	{
		if (layer_index >= layers.size())
			return false;
		layers[layer_index].Pause();
		return true;
	}

	bool Animator::Stop(int layer_index)
	{
		if (layer_index >= layers.size())
			return false;

		layers[layer_index].Stop();
		return true;
	}

	void Animator::PauseAllLayers()
	{
		for (auto& layer : layers)
		{
			layer.Pause();
		}

		preview_playback = false;
	}

	void Animator::StopAllLayers()
	{
		for (auto& layer : layers)
		{
			layer.Stop();
		}
		preview_playback = false;
	}

	void Animator::ResetTriggers()
	{
		for (auto& trigger : parameters.trigger_vars)
			trigger.second.val = false;
	}

	int Animator::GetInt(string_view name) const
	{
		return GetParam<anim::IntParam>(name).val;
	}

	float Animator::GetFloat(string_view name) const
	{
		return GetParam<anim::FloatParam>(name).val;
	}

	bool Animator::GetBool(string_view name) const
	{
		return GetParam<anim::BoolParam>(name).val;
	}

	bool Animator::GetTrigger(string_view name) const
	{
		return GetParam<anim::TriggerParam>(name).val;
	}

	bool Animator::SetInt(string_view name, int val)
	{
		return SetParam<anim::IntParam>(name, val);
	}

	bool Animator::SetFloat(string_view name, float val)
	{
		return SetParam<anim::FloatParam>(name, val);
	}

	bool Animator::SetBool(string_view name, bool val)
	{
		return SetParam<anim::BoolParam>(name, val);
	}

	bool Animator::SetTrigger(string_view name, bool val)
	{
		return SetParam<anim::TriggerParam>(name, val);
	}

	bool Animator::SetWeight(float weight, int layer_index)
	{
		if (layer_index == 0 || layer_index >= layers.size())
		{
			return false;
		}
		layers[layer_index].weight = weight;
		return true;
	}

	bool Animator::HasState(string_view name, int layer_index) const
	{
		if (layer_index >= layers.size())
			return false;

		return layers[layer_index].HasState(name);
	}

	bool Animator::IsPlaying(int layer_index) const
	{
		if (layer_index >= layers.size())
			return false;
		return layers[layer_index].IsPlaying();
	}

	bool Animator::IsBlending(int layer_index) const
	{
		if (layer_index >= layers.size())
			return false;
		return layers[layer_index].IsBlending();
	}

	bool Animator::HasCurrAnimEnded(int layer_index) const
	{
		if (layer_index >= layers.size())
			return false;
		return layers[layer_index].HasCurrAnimEnded();
	}

	CSharpState Animator::GetState(string_view name, int layer_index) const
	{
		if (layer_index >= layers.size())
			return CSharpState{};

		const auto& layer = layers[layer_index];
		auto index = layer.FindAnimationIndex(name);
		auto& state = layer.GetAnimationState(index);
		if (!state.valid)
			return CSharpState{};

		CSharpState cs_state;
		cs_state.valid = state.valid;
		cs_state.loop = state.loop;

		cs_state.normalizedTime = 0.0f;
		if (index == layer.curr_state.index)
			cs_state.normalizedTime = layer.curr_state.normalized_time;
		else if(index == layer.blend_state.index)
			cs_state.normalizedTime = layer.blend_state.normalized_time;

		auto state_data = state.GetBasicState();
		if (state_data)
		{
			cs_state.duration = state_data->motion->_duration;
			cs_state.fps = state_data->motion->_fps;
		}
		
		return cs_state;
	}

	float Animator::GetWeight(int layer_index) const
	{
		if (layer_index >= layers.size())
			return 0.0f;
		return layers[layer_index].weight;
	}

	string Animator::DefaultStateName(int layer_index) const
	{
		if (layer_index >= layers.size())
			return "";
		return layers[layer_index].DefaultStateName();
	}

	string Animator::CurrentStateName(int layer_index) const
	{
		if (layer_index >= layers.size())
			return "";
		return layers[layer_index].CurrentStateName();
	}

	float Animator::CurrentStateTime(int layer_index) const
	{
		if (layer_index >= layers.size())
			return 0.0f;
		return layers[layer_index].curr_state.normalized_time;
	}

	float Animator::CurrentStateElapsed(int layer_index) const
	{
		if (layer_index >= layers.size())
			return 0.0f;
		return layers[layer_index].curr_state.normalized_time;
	}

	string Animator::BlendStateName(int layer_index) const
	{
		if (layer_index >= layers.size())
			return "";
		return layers[layer_index].BlendStateName();
	}

	float Animator::BlendStateTime(int layer_index) const
	{
		if (layer_index >= layers.size())
			return 0.0f;
		return layers[layer_index].blend_state.normalized_time;
	}

	float Animator::BlendStateElapsed(int layer_index) const
	{
		if (layer_index >= layers.size())
			return 0.0f;
		return layers[layer_index].blend_state.elapsed_time;
	}

#pragma endregion

	void Animator::on_parse()
	{
		return;
		// Core::GetSystem<AnimationSystem>().
		// const auto scene = Core::GetSystem<SceneManager>().GetSceneByBuildIndex(GetHandle().scene);
		// auto* sg = Core::GetSystem<SceneManager>().FetchSceneGraphFor(GetGameObject());
		// 
		// if (skeleton)
		// {
		// 	size_t num_bones = skeleton->data().size();
		// 	_bind_pose.resize(num_bones);
		// 	_child_objects.resize(num_bones);
		// 	pre_global_transforms.resize(num_bones);
		// 	final_bone_transforms.resize(num_bones);
		// }
		// 
		// auto& child_objects = _child_objects;
		// const auto initialize_children =
		// 	[&child_objects](Handle<GameObject> c_go, int)
		// 	{
		// 		auto c_bone = c_go->GetComponent<Bone>();
		// 		if (c_bone)
		// 		{
		// 			child_objects[c_bone->bone_index] = c_go;
		// 		}
		// 	};
		// 
		// sg->visit(initialize_children);
		// Core::GetSystem<AnimationSystem>().SaveBindPose(*this);
		// 
		// for (auto& layer : layers)
		// {
		// 	layer.playing_index = layer.default_index;
		// 	layer.weight = layer.default_weight;
		// }
	}
}