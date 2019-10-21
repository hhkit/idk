#pragma once

#include <core/Component.h>
#include <anim/Skeleton.h>
#include <anim/Animation.h>
#include <anim/AnimationState.h>
#include <anim/AnimationLayer.h>

namespace idk
{
	class Animator final
		:public Component<Animator> 
	{
	public:
		// Contructor
		Animator();

		// Engine Getters
		const AnimationState& GetAnimationState(string_view name) const;
		RscHandle<anim::Animation> GetAnimationRsc(string_view name) const;

		const vector<mat4>& BoneTransforms();
		
		// Engine Setters
		void AddAnimation(RscHandle<anim::Animation> anim_rsc);
		void RemoveAnimation(string_view name);

		// Editor Functionality
		void Reset();

		// Script Functions
		void Play(string_view animation_name, float offset = 0.0f);
		void Pause();
		void Stop();
		// void Transistion(string_view animation_name);

		// Script Getters
		bool HasState(string_view name) const;
		bool IsPlaying(string_view name) const;
		string GetDefaultState() const;

		// Script Setters
		void SetEntryState(string_view name, float offset = 0.0f);

		// Serialization
		void on_parse();

		// ======================= Public Variables ========================
		RscHandle<anim::Skeleton> skeleton;

		hash_table<string, AnimationState> animation_table;

		hash_table<string, size_t> layer_table{};
		vector<AnimationLayer> layers{};

		// Precomputation step
		vector<mat4> pre_global_transforms{ mat4{} };
		// This is what we send to the graphics system.
		vector<mat4> final_bone_transforms{ mat4{} };
		
	private:
		friend class AnimationSystem;

		// AnimationSystem will update all local transforms for the child objects
		vector<Handle<GameObject>> _child_objects;
		vector<matrix_decomposition<real>> _bind_pose;

		inline static const AnimationState null_state{};
	};
}
