#include "stdafx.h" 
#include <iostream>
#include <idk.h>
#include "common/Transform.h"
#include "core/GameObject.h"
#include "Animator.h"
#include "scene/SceneManager.h"

#include "math/matrix_decomposition.h"

namespace idk 
{
	void Animator::Play(string_view animation_name)
	{
		auto res = _animation_table.find(animation_name.data());
		if (res == _animation_table.end())
		{
			// Maybe throw here???
			std::cout << "Played animation that doesn't exist." << std::endl;
			return;
		}

		// 2 design choices here. Either replay anim no matter what or dont reset if same name
		Play(res->second);
	}

	void Animator::Play(size_t index)
	{
		// 2 design choices here. Either replay anim no matter what or dont reset if same name
		if (index >= _animations.size())
			return;

		if (_curr_animation == index)
		{
			_is_playing = true;
			return;
		}

		saveBindPose();

		_elapsed = 0.0f;
		_curr_animation = s_cast<int>(index);
		_is_playing = true;
	}

	void Animator::Pause()
	{
		_is_playing = false;
	}

	void Animator::Stop()
	{
		restoreBindPose();

		_elapsed = 0.0f;
		_curr_animation = -1;
		_is_playing = false;
	}

	RscHandle<anim::Animation> Animator::GetCurrentAnimation() const
	{
		if (_curr_animation >= _animations.size())
			return RscHandle<anim::Animation>{};

		return _animations[_curr_animation];
	}

	const vector<mat4>& Animator::GenerateTransforms()
	{
		// We grab the parent transform if its there, and multiply it with the child local.
		// We save this result into pre_transforms and then multiply that by global inverse bind pose 
		// and put it into final transforms.
		auto global_inverse = _skeleton->GetGlobalInverse();
		const auto& skeleton = _skeleton->data();
		if (global_inverse != mat4{})
		{
			for (size_t i = 0; i < _child_objects.size(); ++i)
			{
				auto& curr_bone = skeleton[i];
				auto& curr_go = _child_objects[i];

				auto parent_index = skeleton[i]._parent;
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

				auto parent_index = skeleton[i]._parent;
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
				auto test = decompose(_pre_global_transforms[i]);
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
		auto scene = Core::GetSystem<SceneManager>().GetActiveScene();
		
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
			
			mat4 local_bind_pose = curr_bone._local_bind_pose.recompose();

			obj->GetComponent<Transform>()->LocalMatrix(local_bind_pose);
			
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
			_animation_table.emplace(anim_rsc->GetName(), _animations.size());
			_animations.push_back(anim_rsc);
		}
	}

	void Animator::Reset()
	{
		clearGameObjects();

		_skeleton = RscHandle<anim::Skeleton>{};

		_animation_table.clear();
		_animations.clear();

		_elapsed = 0.0f;
		_is_playing = false;
		_curr_animation = -1;
	}

	void Animator::clearGameObjects()
	{
		auto scene = Core::GetSystem<SceneManager>().GetActiveScene();

		for (auto& obj : _child_objects)
		{
			scene->DestroyGameObject(obj);
		}
		_child_objects.clear();
	}

	void Animator::saveBindPose()
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

	void Animator::restoreBindPose()
	{
		// Need to revert back to the bind pose
		const auto& bones = _skeleton->data();
		for (size_t i = 0; i < bones.size(); ++i)
		{
			mat4 local_bind_pose = _bind_pose[i].recompose();

			_child_objects[i]->Transform()->LocalMatrix(local_bind_pose);

			// compute the bone_transform for the bind pose
			auto parent_index = _skeleton->data()[i]._parent;
			if (parent_index >= 0)
			{
				// If we have the parent, we push in the parent.global * child.local
				const mat4& p_transform = _pre_global_transforms[parent_index];
				mat4 final_local_transform = p_transform * local_bind_pose;
				_pre_global_transforms[i] = final_local_transform;
			}
			else
				_pre_global_transforms[i] = local_bind_pose;
		}
	}

}