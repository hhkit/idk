#pragma once

#include <core/Component.h>
#include <anim/Skeleton.h>
#include <anim/Animation.h>

namespace idk
{
	class AnimationController final
		:public Component<AnimationController> 
	{
	public:
		void Play(string_view animation_name);
		void Play(size_t index);
		void Pause();
		void Stop();

		void Transistion(string_view animation_name);

		// Getters
		RscHandle<anim::Animation> GetCurrentAnimation() const;
		const vector<mat4>& GenerateTransforms();
		// Setters
		void SetSkeleton(RscHandle<anim::Skeleton> skeleton_rsc);
		void AddAnimation(RscHandle<anim::Animation> anim_rsc);

		void SetSkeleton(string_view path_to_rsc);
		void AddAnimation(string_view path_to_rsc);

		void Reset();
		
	private:
		friend class AnimationSystem;

		float	_elapsed		= 0.0f;
		int		_curr_animation = -1;
		bool	_is_playing		= false;

		RscHandle<anim::Skeleton> _skeleton;

		hash_table<string_view, size_t> _animation_table;
		vector<RscHandle<anim::Animation>> _animations;

		vector<Handle<GameObject>> _child_objects;
		vector<mat4> _bone_transforms;

		void clearGameObjects();
	};
}
