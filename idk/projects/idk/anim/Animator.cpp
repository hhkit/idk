#include "stdafx.h" 
#include <iostream>
#include <idk.h>
#include "common/Transform.h"
#include "core/GameObject.h"
#include "Animator.h"
#include <math/arith.h>
#include "scene/SceneManager.h"

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

	const vector<mat4>& Animator::GenerateTransforms()
	{
		// We grab the parent transform if its there, and multiply it with the child local.
		// We save this result into pre_transforms and then multiply that by global inverse bind pose 
		// and put it into final transforms.
		const auto global_inverse = _skeleton->GetGlobalInverse();
		const auto& skeleton = _skeleton->data();
		if (global_inverse != mat4{})
		{
			for (size_t i = 0; i < _child_objects.size(); ++i)
			{
				auto& curr_bone = skeleton[i];
				auto& curr_go = _child_objects[i];

				if (!curr_go)
					continue;
				const auto parent_index = skeleton[i]._parent;
				if (parent_index >= 0)
				{
					const mat4& p_transform = _pre_global_transforms[parent_index];
					_pre_global_transforms[i] = p_transform * curr_go->Transform()->LocalMatrix();
				}
				else
				{
					_pre_global_transforms[i] = curr_go->Transform()->LocalMatrix();
				}

				_final_bone_transforms[i] = global_inverse * _pre_global_transforms[i] * curr_bone._global_inverse_bind_pose;
			}
		}
		else
		{
			for (size_t i = 0; i < _child_objects.size(); ++i)
			{
				auto& curr_bone = skeleton[i];
				auto& curr_go = _child_objects[i];

				if (!curr_go)
					continue;
				const auto parent_index = skeleton[i]._parent;
				if (parent_index >= 0)
				{
					// If we have the parent, we push in the parent.global * child.local
					const mat4& p_transform = _pre_global_transforms[parent_index];
					const mat4& c_transform = curr_go->Transform()->LocalMatrix();
					_pre_global_transforms[i] = p_transform * c_transform;
				}
				else
				{
					_pre_global_transforms[i] = curr_go->Transform()->LocalMatrix();
				}
				const auto test = decompose(_pre_global_transforms[i]);
				_final_bone_transforms[i] = _pre_global_transforms[i] * curr_bone._global_inverse_bind_pose;
			}
		}
		return _final_bone_transforms;
	}

	void Animator::SetSkeleton(RscHandle<anim::Skeleton> skeleton_rsc)
	{
		if (!skeleton_rsc)
			return;
		_skeleton = skeleton_rsc;

		// We also need to generate all the game objects here.
		// The game object's transform is the inverse of bone_offset.
		const auto scene = Core::GetSystem<SceneManager>().GetSceneByBuildIndex(GetHandle().scene);

		_pre_global_transforms.clear();
		_final_bone_transforms.clear();
		_bind_pose.clear();
		clearGameObjects();

		const auto& bones = _skeleton->data();
		_bind_pose.resize(bones.size());
		_child_objects.resize(bones.size());
		_pre_global_transforms.resize(bones.size());
		_final_bone_transforms.resize(bones.size());

		for (size_t i = 0; i < bones.size(); ++i)
		{
			auto& curr_bone = bones[i];

			auto obj = scene->CreateGameObject();
			// auto transform = curr_bone._global_inverse_bind_pose.inverse();

			// mat4 local_bind_pose = curr_bone._local_bind_pose.recompose();

			obj->GetComponent<Transform>()->position = curr_bone._local_bind_pose.position;
			obj->GetComponent<Transform>()->rotation = curr_bone._local_bind_pose.rotation;
			obj->GetComponent<Transform>()->scale = curr_bone._local_bind_pose.scale;

			obj->Name(curr_bone._name);

			if (curr_bone._parent >= 0)
				obj->GetComponent<Transform>()->SetParent(_child_objects[curr_bone._parent], false);
			else
				obj->GetComponent<Transform>()->SetParent(GetGameObject(), false);

			_bind_pose[i] = curr_bone._local_bind_pose;
			_child_objects[i] = obj;
		}
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

	void Animator::SaveBindPose()
	{
		// Need to save the local transforms of the child objects as bind pose
		for (size_t i = 0; i < _child_objects.size(); ++i)
		{
			auto curr_go = _child_objects[i];
			_bind_pose[i].position = curr_go->Transform()->position;
			_bind_pose[i].rotation = curr_go->Transform()->rotation;
			_bind_pose[i].scale = curr_go->Transform()->scale;
		}
	}

	void Animator::RestoreBindPose()
	{
		// Need to revert back to the bind pose
		// const auto& bones = _skeleton->data();
		for (size_t i = 0; i < _bind_pose.size(); ++i)
		{
			auto local_bind_pose = _bind_pose[i];
			auto local_bind_pose_mat = local_bind_pose.recompose();

			_child_objects[i]->Transform()->position = local_bind_pose.position;
			_child_objects[i]->Transform()->rotation = local_bind_pose.rotation;
			_child_objects[i]->Transform()->scale = local_bind_pose.scale;

			// compute the bone_transform for the bind pose
			const auto parent_index = _skeleton->data()[i]._parent;
			if (parent_index >= 0)
			{
				// If we have the parent, we push in the parent.global * child.local
				const mat4& p_transform = _pre_global_transforms[parent_index];
				mat4 final_local_transform = p_transform * local_bind_pose_mat;
				_pre_global_transforms[i] = final_local_transform;
			}
			else
				_pre_global_transforms[i] = local_bind_pose_mat;
		}
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

	void Animator::clearGameObjects()
	{
		const auto scene = Core::GetSystem<SceneManager>().GetSceneByBuildIndex(GetHandle().scene);

		for (auto& obj : _child_objects)
		{
			scene->DestroyGameObject(obj);
		}
		_child_objects.clear();
	}

	
}