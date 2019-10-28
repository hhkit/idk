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
	AnimationState& Animator::GetAnimationState(string_view name)
	{
		auto res = animation_table.find(name.data());
		if (res != animation_table.end())
			return res->second;

		return null_state;
	}

	const AnimationState& Animator::GetAnimationState(string_view name) const
	{
		auto res = animation_table.find(name.data());
		if (res != animation_table.end())
			return res->second;

		return null_state;
	}

	const vector<mat4>& Animator::BoneTransforms()
	{
		return final_bone_transforms;
	}

	void Animator::AddAnimation(RscHandle<anim::Animation> anim_rsc)
	{
		if (anim_rsc)
		{
			string name = anim_rsc->Name().data();
			// Check if name exists
			if (animation_table.find(name) != animation_table.end())
			{
				string append = " 0";
				int count = 1;
				while (animation_table.find(name + append) != animation_table.end())
				{
					// Generate a unique name
					append = " " + std::to_string(count);
				}
				name += append;
			}
			AnimationState state{ name, true };
			state.state_data = variant<BasicAnimationState, BlendTree>{ BasicAnimationState{ anim_rsc } };
			animation_table.emplace(name, state);
		}
		else
		{
			string name = "New State ";
			string append = "0";
			// Check if name exists
			int count = 1;
			while (animation_table.find(name + append) != animation_table.end())
			{
				// Generate a unique name
				append = " " + std::to_string(count);
			}
			name += append;
			animation_table.emplace(name, AnimationState{ name, true });
		}
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
			append = " " + std::to_string(count);
		}

		AnimationLayer new_layer{};
		new_layer.name = name + append;
		new_layer.prev_poses.resize(skeleton->data().size());
		new_layer.blend_source.resize(skeleton->data().size());

		layer_table.emplace(new_layer.name, layers.size());
		layers.push_back(new_layer);
	}

	void Animator::RemoveAnimation(string_view name)
	{
		auto found_clip = animation_table.find(string{ name });
		if (found_clip == animation_table.end())
			return;

		for (auto& layer : layers)
		{
			if (layer.curr_state.name == found_clip->second.name)
				layer.curr_state = AnimationLayerState{};
			if (layer.default_state == found_clip->second.name)
				layer.default_state = string{};
		}

		animation_table.erase(found_clip);
	}

#pragma endregion

#pragma region Editor Functionality
	void Animator::Reset()
	{
		preview_playback = false;
		for(auto& layer : layers)
			layer.Reset();
	}

	void Animator::OnPreview()
	{
		if (preview_playback)
		{
			for (size_t i = 0; i < layers.size(); ++i)
			{
				Play(layers[i].curr_state.name, i);
			}
		}
		else
		{
			for (size_t i = 0; i < layers.size(); ++i)
			{
				layers[i].Reset();
			}
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
		layers[0].BlendTo(animation_name, std::min(time, 1.0f));
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

	int Animator::GetInt(string_view name) const
	{
		UNREFERENCED_PARAMETER(name);
		return false;
	}

	bool Animator::GetBool(string_view name) const
	{
		UNREFERENCED_PARAMETER(name);
		return false;
	}

	float Animator::GetFloat(string_view name) const
	{
		UNREFERENCED_PARAMETER(name);
		return false;
	}

	bool Animator::HasState(string_view name) const
	{
		return animation_table.find(string{ name }) != animation_table.end();
	}

	bool Animator::IsPlaying() const
	{
		return layers[0].IsPlaying();
	}

	bool Animator::IsBlending() const
	{
		return layers[0].blend_state.is_playing;
	}

	bool Animator::HasCurrAnimEnded() const
	{
		return layers[0].curr_state.normalized_time >= 1.0f;
	}

	string Animator::DefaultStateName() const
	{
		return layers[0].default_state;
	}

	string Animator::CurrentStateName() const
	{
		return layers[0].curr_state.name;
	}

	string Animator::BlendStateName() const
	{
		return layers[0].blend_state.name;
	}

	bool Animator::SetInt(string_view name, int val)
	{
		UNREFERENCED_PARAMETER(name);
		UNREFERENCED_PARAMETER(val);
		return false;
	}

	bool Animator::SetBool(string_view name, bool val)
	{
		UNREFERENCED_PARAMETER(name);
		UNREFERENCED_PARAMETER(val);
		return false;
	}

	bool Animator::SetFloat(string_view name, float val)
	{
		UNREFERENCED_PARAMETER(name);
		UNREFERENCED_PARAMETER(val);
		return false;
	}

	void Animator::SetEntryState(string_view name, float)
	{
		auto res = animation_table.find(name.data());
		if (res == animation_table.end())
		{
			return;
		}

		layers[0].default_state = name;
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
		// 			child_objects[c_bone->_bone_index] = c_go;
		// 		}
		// 	};
		// 
		// sg->visit(initialize_children);
		// Core::GetSystem<AnimationSystem>().SaveBindPose(*this);
		// 
		// for (auto& layer : layers)
		// {
		// 	layer.curr_state = layer.default_state;
		// 	layer.weight = layer.default_weight;
		// }
	}
}