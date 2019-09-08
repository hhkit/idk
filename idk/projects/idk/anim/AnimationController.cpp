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
		_curr_animation = res->second;
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
		return _animations[_curr_animation];
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
			auto transform = decompose(elem._offset.inverse());
			
			obj->GetComponent<Transform>()->position	= transform.position;
			obj->GetComponent<Transform>()->scale		= transform.scale;
			obj->GetComponent<Transform>()->rotation	= transform.rotation;

			if (elem._parent >= 0)
				obj->GetComponent<Transform>()->parent = _child_objects[elem._parent];

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