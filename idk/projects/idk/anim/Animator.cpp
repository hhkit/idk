#include "stdafx.h" 
#include <iostream>
#include <idk.h>
#include <anim/Bone.h>
#include "common/Transform.h"
#include "core/GameObject.h"
#include "Animator.h"
#include <math/arith.h>
#include "scene/SceneManager.h"
#include "AnimationSystem.h"

#include "math/matrix_decomposition.h"

namespace idk 
{
	Animator::Animator()
	{
		// Initialize the base layer. Cannot be removed or edited (much).
		AnimationLayer base_layer;
		base_layer.name = "Base Layer";
		std::fill(base_layer.bone_mask.begin(), base_layer.bone_mask.end(), true);
		
		layer_table.emplace("Base Layer", 0);
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
		string name = "New Layer ";
		string append = "0";
		// Check if name exists
		int count = 1;
		while (layer_table.find(name + append) != layer_table.end())
		{
			// Generate a unique name
			append = " " + std::to_string(count++);
		}

		AnimationLayer new_layer{};
		new_layer.name = name + append;
		new_layer.prev_poses.resize(skeleton->data().size());
		new_layer.blend_source.resize(skeleton->data().size());

		// All bones are initialized to be unmasked the start
		std::fill(new_layer.bone_mask.begin(), new_layer.bone_mask.end(), true);

		layer_table.emplace(new_layer.name, layers.size());
		layers.push_back(new_layer);
	}

	bool Animator::RenameLayer(string_view from, string_view to)
	{
		string from_str{ from };
		string to_str{ to };
		auto res = layer_table.find(from_str);
		if (res == layer_table.end())
		{
			LOG_TO(LogPool::ANIM, string{ "Cannot rename animation layer (" } +from.data() + ") to (" + to.data() + ").");
			return false;
		}

		auto found_dest = layer_table.find(to_str);
		if (found_dest != layer_table.end())
		{
			LOG_TO(LogPool::ANIM, string{ "Cannot rename animation layer (" } +from.data() + ") to (" + to.data() + ").");
			return false;
		}


		auto copy = res->second;

		layer_table.erase(res);
		layer_table.emplace(to_str, copy);

		layers[copy].name = to_str;

		return true;
	}

	void Animator::RemoveLayer(string_view name)
	{
		auto res = layer_table.find(name.data());
		if (res == layer_table.end())
		{
			LOG_TO(LogPool::ANIM, string{ "Cannot delete animation layer (" } + name.data() + ".");
			return;
		}

		if (res->second >= layers.size())
		{
			LOG_TO(LogPool::ANIM, string{ "Cannot delete animation layer (" } + name.data() + ".");
			return;
		}

		layers.erase(layers.begin() + res->second);
		layer_table.erase(res);
	}

	void Animator::RemoveLayer(size_t index)
	{
		if(index >= layers.size())
		{
			LOG_TO(LogPool::ANIM, string{ "Cannot delete animation layer (" } + std::to_string(index) + ".");
			return;
		}

		auto res = layer_table.find(layers[index].name.data());
		if (res == layer_table.end())
		{
			LOG_TO(LogPool::ANIM, string{ "Cannot delete animation layer (" } + layers[index].name.data() + ".");
			return;
		}

		layer_table.erase(res);
		layers.erase(layers.begin() + index);
	}

#pragma endregion

#pragma region Editor Functionality
	void Animator::Reset()
	{
		preview_playback = false;
		for(auto& layer : layers)
			layer.Reset();

		for (auto& p : int_vars)
			p.second.ResetToDefault();

		for (auto& p : float_vars)
			p.second.ResetToDefault();

		for (auto& p : bool_vars)
			p.second.ResetToDefault();

		for (auto& p : trigger_vars)
			p.second.ResetToDefault();
	}

	void Animator::ResetToDefault()
	{
		for (size_t i = 0; i < layers.size(); ++i)
		{
			layers[i].ResetToDefault();
		}

		for (auto& p : int_vars)
			p.second.ResetToDefault();

		for (auto& p : float_vars)
			p.second.ResetToDefault();

		for (auto& p : bool_vars)
			p.second.ResetToDefault();

		for (auto& p : trigger_vars)
			p.second.ResetToDefault();
	}

	void Animator::OnPreview()
	{
		if (preview_playback)
		{
			for (size_t i = 0; i < layers.size(); ++i)
			{
				layers[i].Play(layers[i].curr_state.index);
			}
		}
		else
		{
			ResetToDefault();
			Core::GetSystem<AnimationSystem>().RestoreBindPose(*this);
		}
	}

#pragma endregion

#pragma region Script Functions
	void Animator::Play(string_view animation_name, float offset)
	{
		layers[0].Play(animation_name, offset);
	}


	void Animator::Play(string_view animation_name, string_view layer_name, float offset)
	{
		// bool valid = true;
		auto layer_res = layer_table.find(layer_name.data());
		if (layer_res == layer_table.end())
		{
			std::cout << "[Animator] Animation Layer (" + string{ layer_name } +") doesn't exist." << std::endl;
			return;
		}

		layers[layer_res->second].Play(animation_name, offset);
	}

	void Animator::Play(string_view animation_name, size_t layer_index, float offset)
	{
		// bool valid = true;
		if (s_cast<size_t>(layer_index) >= layers.size())
		{
			std::cout << "[Animator] Animation Layer index (" + std::to_string(layer_index) +") doesn't exist." << std::endl;
			// valid = false;
			return;
		}

		layers[layer_index].Play(animation_name, offset);
	}

	void Animator::BlendTo(string_view animation_name, float time)
	{
		// Cap blend duration to 1.0f
		time = std::min(abs(time), 1.0f);

		// If time is 0.0, we just call play cos its the same.
		if(time < 0.00001f)
			layers[0].Play(animation_name);
		else
			layers[0].BlendTo(animation_name, time);
	}

	void Animator::Resume()
	{
		layers[0].Resume();
	}

	void Animator::Resume(string_view layer_name)
	{
		auto layer_res = layer_table.find(layer_name.data());
		if (layer_res == layer_table.end())
		{
			std::cout << "[Animator] Animation Layer (" + string{ layer_name } +") doesn't exist." << std::endl;
			return;
		}

		layers[layer_res->second].Resume();
	}

	void Animator::Resume(size_t layer_index)
	{
		if (s_cast<size_t>(layer_index) >= layers.size())
		{
			std::cout << "[Animator] Animation Layer index (" + std::to_string(layer_index) + ") doesn't exist." << std::endl;
			// valid = false;
			return;
		}
		layers[layer_index].Resume();
	}

	void Animator::Pause()
	{
		layers[0].Pause();
		preview_playback = false;
	}

	void Animator::Pause(string_view layer_name)
	{
		auto layer_res = layer_table.find(layer_name.data());
		if (layer_res == layer_table.end())
		{
			std::cout << "[Animator] Animation Layer (" + string{ layer_name } +") doesn't exist." << std::endl;
			return;
		}

		layers[layer_res->second].Pause();
	}

	void Animator::Pause(int layer_index)
	{
		if (s_cast<size_t>(layer_index) >= layers.size())
		{
			std::cout << "[Animator] Animation Layer index (" + std::to_string(layer_index) + ") doesn't exist." << std::endl;
			return;
		}

		layers[layer_index].Pause();
	}

	void Animator::Stop()
	{
		layers[0].Stop();
	}

	void Animator::Stop(string_view layer_name)
	{
		auto layer_res = layer_table.find(layer_name.data());
		if (layer_res == layer_table.end())
		{
			std::cout << "[Animator] Animation Layer (" + string{ layer_name } +") doesn't exist." << std::endl;
			return;
		}

		layers[layer_res->second].Stop();
	}

	void Animator::Stop(int layer_index)
	{
		if (s_cast<size_t>(layer_index) >= layers.size())
		{
			std::cout << "[Animator] Animation Layer index (" + std::to_string(layer_index) + ") doesn't exist." << std::endl;
			return;
		}

		layers[layer_index].Stop();
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
		for (auto& trigger : trigger_vars)
			trigger.second.val = false;
	}

	int Animator::GetInt(string_view name) const
	{
		auto res = int_vars.find(name.data());
		if (res != int_vars.end())
			return res->second.val;

		return 0;
	}

	float Animator::GetFloat(string_view name) const
	{
		auto res = float_vars.find(name.data());
		if (res != float_vars.end())
			return res->second.val;

		return 0.0f;
	}

	bool Animator::GetBool(string_view name) const
	{
		auto res = bool_vars.find(name.data());
		if (res != bool_vars.end())
			return res->second.val;

		return false;
	}

	bool Animator::GetTrigger(string_view name) const
	{
		auto res = trigger_vars.find(name.data());
		if (res != trigger_vars.end())
			return res->second.val;

		return false;
	}

	bool Animator::SetInt(string_view name, int val, bool set, bool def_val)
	{
		auto res = int_vars.find(name.data());
		bool found = false;

		if (set)
		{
			anim::AnimationParam<int> param;
			param.name = name.data();
			int count = -1;
			while (res != int_vars.end())
			{
				++count;
				res = int_vars.find(param.name + std::to_string(count));
			}

			if (count >= 0)
				param.name += std::to_string(count);

			param.def_val = val;
			param.val = val;

			int_vars.emplace(param.name, param);
		}
		else if (res != int_vars.end())
		{
			if (def_val)
				res->second.def_val = val;

			res->second.val = val;
			found = true;
		}

		return found;
	}

	bool Animator::SetFloat(string_view name, float val, bool set, bool def_val)
	{
		auto res = float_vars.find(name.data());
		bool found = false;

		if (set)
		{
			anim::AnimationParam<float> param;
			param.name = name.data();

			int count = -1;
			while (res != float_vars.end())
			{
				++count;
				res = float_vars.find(param.name + std::to_string(count));
			}

			if (count >= 0)
				param.name += std::to_string(count);
			
			param.def_val = val;
			param.val = val;

			float_vars.emplace(param.name, param);
		}
		else if (res != float_vars.end())
		{
			if(def_val)
				res->second.def_val = val;

			res->second.val = val;
			found = true;
		}

		return found;
	}

	bool Animator::SetBool(string_view name, bool val, bool set, bool def_val)
	{
		auto res = bool_vars.find(name.data());
		bool found = false;

		if (set)
		{
			anim::AnimationParam<bool> param;
			param.name = name.data();

			int count = -1;
			while (res != bool_vars.end())
			{
				++count;
				res = bool_vars.find(param.name + std::to_string(count));
			}

			if (count >= 0)
				param.name += std::to_string(count);

			param.def_val = val;
			param.val = val;

			bool_vars.emplace(param.name, param);
			found = true;
		}
		else if (res != bool_vars.end())
		{
			if (def_val)
				res->second.def_val = val;

			res->second.val = val;
			found = true;
		} 
		
		return found;
	}

	bool Animator::SetTrigger(string_view name, bool val, bool set, bool def_val)
	{
		auto res = trigger_vars.find(name.data());
		bool found = false;

		if (set)
		{
			anim::AnimationParam<bool> param;
			param.name = name.data();

			int count = -1;
			while (res != trigger_vars.end())
			{
				++count;
				res = trigger_vars.find(param.name + std::to_string(count));
			}

			if (count >= 0)
				param.name += std::to_string(count);

			param.def_val = val;
			param.val = val;

			trigger_vars.emplace(param.name, param);
			found = true;
		}
		else if (res != trigger_vars.end())
		{
			if (def_val)
				res->second.def_val = val;

			res->second.val = val;
			found = true;
		}

		return found;
	}

	bool Animator::HasState(string_view name) const
	{
		return layers[0].HasState(name);
	}

	bool Animator::IsPlaying() const
	{
		return layers[0].IsPlaying();
	}

	bool Animator::IsBlending() const
	{
		return layers[0].IsBlending();
	}

	bool Animator::HasCurrAnimEnded() const
	{
		return layers[0].HasCurrAnimEnded();
	}

	string Animator::DefaultStateName() const
	{
		return layers[0].DefaultStateName();
	}

	string Animator::CurrentStateName() const
	{
		return layers[0].CurrentStateName();
	}

	string Animator::BlendStateName() const
	{
		return layers[0].BlendStateName();
	}


	void Animator::SetEntryState(string_view , float)
	{
		// auto res = animation_table.find(name.data());
		// if (res == animation_table.end())
		// {
		// 	return;
		// }
		// 
		// layers[0].default_index = name;
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