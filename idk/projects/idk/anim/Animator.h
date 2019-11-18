#pragma once

#include <core/Component.h>
#include <anim/Skeleton.h>
#include <anim/Animation.h>
#include <anim/AnimationState.h>
#include <anim/AnimationLayer.h>
#include <anim/AnimationUtils.h>

namespace idk
{
	class Animator final
		:public Component<Animator> 
	{
	public:
		// Contructor
		Animator();

		// Engine Getters
		AnimationState& GetAnimationState(string_view name);
		const AnimationState& GetAnimationState(string_view name) const;
		// RscHandle<anim::Animation> GetAnimationRsc(string_view name) const;

		const vector<mat4>& BoneTransforms()const;
		
		// Engine Setters
		void AddLayer();
		bool RenameLayer(string_view from, string_view to);
		void RemoveLayer(string_view name);
		void RemoveLayer(size_t index);

		void AddAnimation(RscHandle<anim::Animation> anim_rsc);
		bool RenameAnimation(string_view from, string_view to);
		void RemoveAnimation(string_view name);

		// Editor Functionality
		void Reset();
		void ResetToDefault();
		void OnPreview();
		

		// Script Functions
		void Play(string_view animation_name, float offset = 0.0f);
		void Play(string_view animation_name, string_view layer_name, float offset = 0.0f);
		void Play(string_view animation_name, size_t layer_index, float offset = 0.0f);

		void BlendTo(string_view animation_name, float time = 0.2f);

		void Resume();
		void Resume(string_view layer_name);
		void Resume(size_t layer_index);

		void Pause();
		void Pause(string_view layer_name);
		void Pause(int layer_index);

		void Stop();
		void Stop(string_view layer_name);
		void Stop(int layer_index);

		// void ResumeAllLayers();
		void PauseAllLayers();
		void StopAllLayers();
		
		// Script Getters
		string DefaultStateName() const;
		string CurrentStateName() const;
		string BlendStateName() const;

		bool IsPlaying() const;
		bool IsBlending() const;
		bool HasCurrAnimEnded() const;
		bool HasState(string_view name) const;

		int GetInt(string_view name) const;
		float GetFloat(string_view name) const;
		bool GetBool(string_view name) const;
		bool GetTrigger(string_view name) const;

		// Script Setters
		bool SetInt(string_view name, int val, bool set = false, bool def_val = false);
		bool SetFloat(string_view name, float val, bool set = false, bool def_val = false);
		bool SetBool(string_view name, bool val, bool set = false, bool def_val = false);
		bool SetTrigger(string_view name, bool val, bool set = false, bool def_val = false);

		void ResetTriggers();

		void SetEntryState(string_view name, float offset = 0.0f);

		// Serialization
		void on_parse();

		// ======================= Public Variables ========================
		RscHandle<anim::Skeleton> skeleton;

		hash_table<string, size_t> layer_table{};
		vector<AnimationLayer> layers{};

		// Scripting variables
		hash_table<string, anim::AnimationParam<int>> int_vars;
		hash_table<string, anim::AnimationParam<float>> float_vars;
		hash_table<string, anim::AnimationParam<bool>> bool_vars;
		hash_table<string, anim::AnimationParam<bool>> trigger_vars;
		
		// Precomputation step
		vector<mat4> pre_global_transforms{ mat4{} };
		// This is what we send to the graphics system.
		vector<mat4> final_bone_transforms{ mat4{} };
		bool preview_playback = false;
	private:
		friend class AnimationSystem;

		// AnimationSystem will update all local transforms for the child objects
		vector<Handle<GameObject>> _child_objects;
		vector<matrix_decomposition<real>> _bind_pose;
		
		inline static AnimationState null_state{};
	};
}
