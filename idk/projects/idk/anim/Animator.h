#pragma once

#include <core/Component.h>
#include <anim/Skeleton.h>
#include <anim/Animation.h>
#include <anim/AnimationState.h>

namespace idk
{
	class Animator final
		:public Component<Animator> 
	{
	public:
		// Engine Getters
		int GetAnimationIndex(string_view name) const;
		string GetStateName(int id) const;

		const AnimationState& GetAnimationState(string_view name) const;
		const AnimationState& GetAnimationState(int id) const;

		RscHandle<anim::Animation> GetAnimationRsc(string_view name) const;
		RscHandle<anim::Animation> GetAnimationRsc(int id) const;

		const vector<mat4>& GenerateTransforms();
		
		// Engine Setters
		void SetSkeleton(RscHandle<anim::Skeleton> skeleton_rsc);
		void AddAnimation(RscHandle<anim::Animation> anim_rsc);

		// Editor Functionality
		void SaveBindPose();
		void RestoreBindPose();
		void Reset();

		// Script Functions
		void Play(string_view animation_name, float offset = 0.0f);
		void Play(size_t index, float offset = 0.0f);
		void Pause();
		void Stop();
		// void Transistion(string_view animation_name);

		// Script Getters
		bool HasState(string_view name) const;
		bool IsPlaying(string_view name) const;
		string GetEntryState() const;

		// Script Setters
		void SetEntryState(string_view name, float offset = 0.0f);

		// ======================= Public Variables ========================
		RscHandle<anim::Skeleton> _skeleton;

		hash_table<string, size_t> _animation_table;
		vector<AnimationState> _animations;

		bool _preview_playback = false;

		friend class AnimationSystem;

		float	_elapsed = 0.0f;

		// Animation States
		int		_start_animation = -1;
		float	_start_animation_offset = 0.0f;
		int		_curr_animation = -1;

		// Animation Playback controls
		bool	_is_playing = false;
		bool	_is_stopping = false;

		void clearGameObjects();

		// Precomputation step
		vector<mat4> _pre_global_transforms{ mat4{} };
		// This is what we send to the graphics system.
		vector<mat4> _final_bone_transforms{ mat4{} };

		// AnimationSystem will update all local transforms for the child objects
		vector<Handle<GameObject>> _child_objects;
		vector<matrix_decomposition<real>> _bind_pose;

	private:
		static constexpr AnimationState empty_state{};
	};
}
