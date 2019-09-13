#include "stdafx.h" 
#include <iostream>

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

	vector<mat4> AnimationController::GenerateTransforms() const
	{
		vector<mat4> bone_transforms;
		bone_transforms.reserve(_child_objects.size());

		for (size_t i = 0; i < _child_objects.size(); ++i)
		{
			auto& child = _child_objects[i];
			auto parent_index = _skeleton->data()[i]._parent;
			if (parent_index >= 0)
			{
				// If we have the parent, we push in the parent.global * child.local
				mat4 c_transform = child->GetComponent<Transform>()->LocalMatrix(); 
				mat4 p_transform = bone_transforms[parent_index];
				bone_transforms.emplace_back(p_transform *c_transform);
			}
			else
			{
				bone_transforms.emplace_back(
					child->GetComponent<Transform>()->GlobalMatrix()
				);
			}
		}
		return bone_transforms;
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
		for (auto& elem : bones)
		{
			auto obj = scene->CreateGameObject();
			auto transform = elem._offset.inverse();

			obj->GetComponent<Transform>()->GlobalMatrix(transform);

			if (elem._parent >= 0)
				obj->GetComponent<Transform>()->SetParent(_child_objects[elem._parent], true);

			_child_objects.push_back(obj);
		}
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