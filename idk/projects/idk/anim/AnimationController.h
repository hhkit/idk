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
		void Pause();
		void Stop();
		void Transistion(string_view animation_name);

		// Getters
		RscHandle<anim::Animation> GetCurrentAnimation() const;

		// Setters
		void SetSkeleton(RscHandle<anim::Skeleton>);
		void SetSkeleton(string_view path_to_rsc);

		void AddAnimation(RscHandle<anim::Animation>);
		void AddAnimation(string_view path_to_rsc);
		
	private:
		float	_elapsed		= 0.0f;
		int		_curr_animation = -1;

		RscHandle<anim::Skeleton> _skeleton;
		vector<RscHandle<anim::Animation>> _animations;

		vector<Handle<GameObject>> _child_objects;
	};
}
