#include "stdafx.h" 
#include <iostream>
#include <idk.h>
#include "common/Transform.h"
#include "core/GameObject.h"
#include "AnimationController.h"
#include "scene/SceneManager.h"

#include "math/matrix_decomposition.h"

namespace idk 
{
	void AnimationController::Play(string_view animation_name)
	{
		auto res = _animation_table.find(animation_name);
		if (res == _animation_table.end())
		{
			// Maybe throw here???
			std::cout << "Played animation that doesn't exist." << std::endl;
			return;
		}

		// 2 design choices here. Either replay anim no matter what or dont reset if same name
		if (_curr_animation == res->second)
		{
			_is_playing = true;
			return;
		}

		_elapsed = 0.0f;
		_curr_animation = s_cast<int>(res->second);
	_is_playing = true;
	}

	void AnimationController::Play(size_t index)
	{
		// 2 design choices here. Either replay anim no matter what or dont reset if same name
		if (index >= _animations.size())
			return;

		if (_curr_animation == index)
		{
			_is_playing = true;
			return;
		}

		_elapsed = 0.0f;
		_curr_animation = s_cast<int>(index);
		_is_playing = true;
	}

	void AnimationController::Pause()
	{
		_is_playing = false;
	}

	void AnimationController::Stop()
	{
		_elapsed = 0.0f;
		_is_playing = false;
	}

	RscHandle<anim::Animation> AnimationController::GetCurrentAnimation() const
	{
		if (_curr_animation >= _animations.size())
			return RscHandle<anim::Animation>{};

		return _animations[_curr_animation];
	}

	const vector<mat4>& AnimationController::GenerateTransforms()
	{
		for (size_t i = 0; i < _child_objects.size(); ++i)
		{
			// auto& child = _child_objects[i];
			auto parent_index = _skeleton->data()[i]._parent;
			if (parent_index >= 0)
			{
				// If we have the parent, we push in the parent.global * child.local
				const mat4& c_transform = _bone_transforms[i];
				const mat4& p_transform = _bone_transforms[parent_index];
				mat4 final_local_transform = p_transform * c_transform;
				_bone_transforms[i] = final_local_transform;
			}
			// else
			// {
			// 	_bone_transforms[i] = child->GetComponent<Transform>()->GlobalMatrix();
			// }
		}

		// Apply offsets to all the transforms
		const auto& skeleton = _skeleton->data();
		if (_skeleton->GetGlobalInverse() != mat4{})
		{
			for (size_t i = 0; i < _child_objects.size(); ++i)
			{
				auto& curr_bone = skeleton[i];
				_bone_transforms[i] = _skeleton->GetGlobalInverse() * _bone_transforms[i] * curr_bone._offset;
			}
		}
		else
		{
			for (size_t i = 0; i < _child_objects.size(); ++i)
			{
				auto& curr_bone = skeleton[i];
				_bone_transforms[i] = _bone_transforms[i] * curr_bone._offset;
			}
		}

		return _bone_transforms;
	}

	void AnimationController::SetSkeleton(RscHandle<anim::Skeleton> skeleton_rsc)
	{
		if (!skeleton_rsc)
			return;
		_skeleton = skeleton_rsc;

		// We also need to generate all the game objects here.
		// The game object's transform is the inverse of bone_offset.
		auto scene = Core::GetSystem<SceneManager>().GetActiveScene();

		clearGameObjects();

		const auto& bones = _skeleton->data();
		for (size_t i = 0; i < bones.size(); ++i)
		{
			auto& curr_bone = bones[i];

			auto obj = scene->CreateGameObject();
			auto transform = curr_bone._offset.inverse();

			obj->GetComponent<Transform>()->GlobalMatrix(transform);

			obj->Name(curr_bone._name);

			if (curr_bone._parent >= 0)
				obj->GetComponent<Transform>()->SetParent(_child_objects[curr_bone._parent], true);
			else
				obj->GetComponent<Transform>()->SetParent(GetGameObject(), true);

			_child_objects.push_back(obj);
		}

		_bone_transforms.resize(bones.size());
	}

	void AnimationController::AddAnimation(RscHandle<anim::Animation> anim_rsc)
	{
		if (anim_rsc)
		{
			_animation_table.emplace(anim_rsc->GetName(), _animations.size());
			_animations.push_back(anim_rsc);
		}
	}

	void AnimationController::Reset()
	{
		clearGameObjects();

		_skeleton = RscHandle<anim::Skeleton>{};

		_animation_table.clear();
		_animations.clear();

		_elapsed = 0.0f;
		_is_playing = false;
		_curr_animation = -1;
	}

	void AnimationController::clearGameObjects()
	{
		auto scene = Core::GetSystem<SceneManager>().GetActiveScene();

		for (auto& obj : _child_objects)
		{
			scene->DestroyGameObject(obj);
		}
		_child_objects.clear();
	}

}