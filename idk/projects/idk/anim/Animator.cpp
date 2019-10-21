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
		layer_table.emplace("Base Layer", 0);
		AnimationLayer base_layer;
		base_layer.name = "Base Layer";
		std::fill(base_layer.bone_mask.begin(), base_layer.bone_mask.end(), true);
		layers.push_back(base_layer);
	}

#pragma region Engine Getters/Setters


	const AnimationState& Animator::GetAnimationState(string_view name) const
	{
		auto res = animation_table.find(name.data());
		if (res != animation_table.end())
			return res->second;

		return null_state;
	}

	RscHandle<anim::Animation> Animator::GetAnimationRsc(string_view name) const
	{
		auto res = animation_table.find(name.data());
		if (res != animation_table.end())
			return res->second.animation;

		return RscHandle<anim::Animation>{};
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
				while (animation_table.find(name + append) == animation_table.end())
				{
					// Generate a unique name
					append = " " + std::to_string(count);
				}
				name += append;
			}
			animation_table.emplace(name, AnimationState{ name, anim_rsc, true });
		}
		else
		{
			string name = "New State ";
			string append = "0";
			// Check if name exists
			int count = 1;
			while (animation_table.find(name + append) == animation_table.end())
			{
				// Generate a unique name
				append = " " + std::to_string(count);
			}
			name += append;
			animation_table.emplace(name, AnimationState{ name, anim_rsc, true });
		}
	}

	void Animator::RemoveAnimation(string_view name)
	{
		auto found_clip = animation_table.find(string{ name });
		if (found_clip == animation_table.end())
			return;

		for (auto& layer : layers)
		{
			if (layer.curr_state == found_clip->second.name)
				layer.curr_state = {};
			if (layer.default_state == found_clip->second.name)
				layer.default_state = {};
		}

		animation_table.erase(found_clip);
	}

#pragma endregion

#pragma region Editor Functionality
	void Animator::Reset()
	{
		layers[0].Reset();
	}

#pragma endregion

#pragma region Script Functions
	void Animator::Play(string_view animation_name, float offset)
	{
		auto res = animation_table.find(animation_name.data());
		if (res == animation_table.end())
		{
			std::cout << "[Animator] Played animation (" + string{ animation_name } +") doesn't exist." << std::endl;
			return;
		}

		layers[0].Play(animation_name, offset);
	}

	void Animator::Pause()
	{
		layers[0].Pause();
	}

	void Animator::Stop()
	{
		layers[0].Stop();
	}

	bool Animator::HasState(string_view name) const
	{
		return animation_table.find(string{ name }) != animation_table.end();
	}

	bool Animator::IsPlaying(string_view name) const
	{
		return layers[0].IsPlaying(name);
	}

	string Animator::GetDefaultState() const
	{
		return layers[0].default_state;
	}

	void Animator::SetEntryState(string_view name, float offset)
	{
		auto res = animation_table.find(name.data());
		if (res == animation_table.end())
		{
			return;
		}

		layers[0].default_state = name;
		layers[0].default_offset = offset;

	}
#pragma endregion

	void Animator::on_parse()
	{
		const auto scene = Core::GetSystem<SceneManager>().GetSceneByBuildIndex(GetHandle().scene);
		auto* sg = Core::GetSystem<SceneManager>().FetchSceneGraphFor(GetGameObject());

		if (skeleton)
		{
			size_t num_bones = skeleton->data().size();
			_bind_pose.resize(num_bones);
			_child_objects.resize(num_bones);
			pre_global_transforms.resize(num_bones);
			final_bone_transforms.resize(num_bones);
		}
		
		auto& child_objects = _child_objects;
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
		Core::GetSystem<AnimationSystem>().SaveBindPose(*this);
	}
}