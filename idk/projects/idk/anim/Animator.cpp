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
#pragma region Engine Getters/Setters

	string Animator::GetStateName(int id) const
	{
		if (id < 0 || id >= _animations.size())
			return string{};

		return string{ _animations[id].animation->Name() };
	}

	int Animator::GetAnimationIndex(string_view name) const
	{
		auto res = _animation_table.find(name.data());
		if (res == _animation_table.end())
			return s_cast<int>(res->second);
		return -1;
	}

	const AnimationState& Animator::GetAnimationState(string_view name) const
	{
		auto res = _animation_table.find(name.data());
		if (res == _animation_table.end())
			return _animations[res->second];

		return empty_state;
	}

	const AnimationState& Animator::GetAnimationState(int id) const
	{
		if (id < 0 || id >= _animations.size())
			return empty_state;

		return _animations[id];
	}

	RscHandle<anim::Animation> Animator::GetAnimationRsc(string_view name) const
	{
		auto res = _animation_table.find(name.data());
		if (res == _animation_table.end())
			return _animations[res->second].animation;

		return RscHandle<anim::Animation>{};
	}

	RscHandle<anim::Animation> Animator::GetAnimationRsc(int id) const
	{
		if (id < 0 || id >= _animations.size())
			return RscHandle<anim::Animation>{};

		return _animations[id].animation;
	}

	const vector<mat4>& Animator::BoneTransforms()
	{
		return _final_bone_transforms;
	}

	void Animator::AddAnimation(RscHandle<anim::Animation> anim_rsc)
	{
		if (anim_rsc)
		{
			if (_animation_table.find(string{ anim_rsc->Name() }) != _animation_table.end())
				return;

			_animation_table.emplace(anim_rsc->Name(), _animations.size());
			_animations.push_back(AnimationState{ anim_rsc });
		}
	}

	void Animator::RemoveAnimation(string_view name)
	{
		auto found_clip = _animation_table.find(string{ name });
		if (found_clip == _animation_table.end())
			return;

		if (_curr_animation == found_clip->second)
			_curr_animation = -1;
		if (_start_animation == found_clip->second)
			_start_animation = -1;

		_animations.erase(_animations.begin() + found_clip->second);
		_animation_table.erase(found_clip);
		
		// Reorder the elements in the table
		for (size_t i = 0; i < _animations.size(); ++i)
			_animation_table[_animations[i].animation->Name().data()] = i;
	}

#pragma endregion

#pragma region Editor Functionality
	void Animator::Reset()
	{
		_elapsed = 0.0f;
		_curr_animation = _start_animation;
		_is_playing = false;
		_is_stopping = false;
		_preview_playback = false;
	}

#pragma endregion

#pragma region Script Functions
	void Animator::Play(string_view animation_name, float offset)
	{
		auto res = _animation_table.find(animation_name.data());
		if (res == _animation_table.end())
		{
			// Maybe throw here???
			std::cout << "Played animation that doesn't exist." << std::endl;
			return;
		}

		// 2 design choices here. Either replay anim no matter what or dont reset if same name
		Play(res->second, offset);
	}

	void Animator::Play(size_t index, float offset)
	{
		// 2 design choices here. Either replay anim no matter what or dont reset if same name
		if (index >= _animations.size())
			return;

		if (_curr_animation == index)
		{
			_is_playing = true;
			return;
		}

		_elapsed = _animations[index].animation->GetDuration() * offset;
		if (offset > 1)
			_elapsed = fmod(_elapsed, _animations[index].animation->GetDuration());
		_curr_animation = s_cast<int>(index);
		_is_playing = true;
	}

	void Animator::Pause()
	{
		_is_playing = false;
		_preview_playback = false;
	}

	void Animator::Stop()
	{
		_elapsed = 0.0f;
		_is_stopping = true;
	}

	void Animator::Loop(bool to_loop)
	{
		_is_looping = to_loop;
	}

	bool Animator::HasState(string_view name) const
	{
		return _animation_table.find(string{ name }) != _animation_table.end();
	}

	bool Animator::IsPlaying(string_view name) const
	{
		return _is_playing && GetAnimationIndex(name) == _curr_animation;
	}

	string Animator::GetEntryState() const
	{
		return GetAnimationState(_start_animation).animation->Name().data();
	}

	void Animator::SetEntryState(string_view name, float offset)
	{
		auto res = _animation_table.find(name.data());
		if (res == _animation_table.end())
		{
			return;
		}

		_start_animation = s_cast<int>(res->second);
		_start_animation_offset = offset;
		_curr_animation = _start_animation;
	}
#pragma endregion

	void Animator::on_parse()
	{
		const auto scene = Core::GetSystem<SceneManager>().GetSceneByBuildIndex(GetHandle().scene);
		auto* sg = Core::GetSystem<SceneManager>().FetchSceneGraphFor(GetGameObject());

		if (_skeleton)
		{
			size_t num_bones = _skeleton->data().size();
			_bind_pose.resize(num_bones);
			_child_objects.resize(num_bones);
			_pre_global_transforms.resize(num_bones);
			_final_bone_transforms.resize(num_bones);
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