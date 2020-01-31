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
		
		const vector<Handle<GameObject>>& GetChildObjects() const { return _child_objects; };

		const vector<mat4>& BoneTransforms()const;
		
		// Engine Setters
		void AddLayer();
		size_t FindLayerIndex(string_view name);
		bool RenameLayer(string_view from, string_view to);
		bool RemoveLayer(string_view name);
		bool RemoveLayer(size_t index);

		void AddAnimation(RscHandle<anim::Animation> anim_rsc);
		bool RenameAnimation(string_view from, string_view to);
		void RemoveAnimation(string_view name);

		template<typename T>
		hash_table<string, T>& GetParamTable();

		template<typename T>
		const hash_table<string, T>& GetParamTable() const;

		template<typename T>
		T& GetParam(string_view name);

		template<typename T>
		const T& GetParam(string_view name) const;

		template<typename T>
		void AddParam(string_view name);

		template<typename ParamType, typename ValueType>
		bool SetParam(string_view name, ValueType val, bool def_val = false);

		template<typename T>
		bool RemoveParam(string_view name);

		template<typename T>
		bool RenameParam(string_view from, string_view to);

		// Editor Functionality
		void Reset();
		void ResetToDefault();
		void OnPreview();

		// Script Functions
		// void Play(string_view animation_name = "", float offset = 0.0f);
		// void Play(string_view animation_name, string_view layer_name, float offset = 0.0f);
		// void Resume();
		// void Resume(string_view layer_name);
		// void Pause();
		// void Pause(string_view layer_name);
		// void Stop();
		// void Stop(string_view layer_name);

		bool Play(string_view animation_name = 0, int layer_index = 0, float offset = 0.0f);
		bool BlendTo(string_view animation_name, int layer_index = 0, float time = 0.2f);
		bool Resume(int layer_index = 0);
		bool Pause(int layer_index = 0);
		bool Stop(int layer_index = 0);

		// void ResumeAllLayers();
		void PauseAllLayers();
		void StopAllLayers();
		
		// Script Getters
		int GetInt(string_view name) const;
		float GetFloat(string_view name) const;
		bool GetBool(string_view name) const;
		bool GetTrigger(string_view name) const;

		CSharpState GetState(string_view name, int layer_index) const;
		float GetWeight(float weight, int layer_index = 0) const;

		string DefaultStateName(int layer_index = 0) const;
		string CurrentStateName(int layer_index = 0) const;
		float CurrentStateTime(int index = 0) const;
		float CurrentStateElapsed(int index = 0) const;
		string BlendStateName(int index = 0) const;
		float BlendStateTime(int index = 0) const;
		float BlendStateElapsed(int index = 0) const;

		bool HasState(string_view name, int index = 0) const;
		bool IsPlaying(int index = 0) const;
		bool IsBlending(int index = 0) const;

		bool HasCurrAnimEnded(int index = 0) const;

		// Script Setters
		bool SetInt(string_view name, int val);
		bool SetFloat(string_view name, float val);
		bool SetBool(string_view name, bool val);
		bool SetTrigger(string_view name, bool val);

		// Layers
		bool SetWeight(float weight, int layer_index = 0);

		void ResetTriggers();

		// Serialization
		void on_parse();

		// ======================= Public Variables ========================
		RscHandle<anim::Skeleton> skeleton;

		// hash_table<string, size_t> layer_table{};
		vector<AnimationLayer> layers{};

		// Scripting variables (Ideally should type erase them)
		struct AnimationParams 
		{
			hash_table<string, anim::IntParam>		int_vars;
			hash_table<string, anim::FloatParam>	float_vars;
			hash_table<string, anim::BoolParam>		bool_vars;
			hash_table<string, anim::TriggerParam>	trigger_vars;
		}parameters{};
		
		bool preview_playback = false;
	private:
		friend class AnimationSystem;

		// AnimationSystem will update all local transforms for the child objects
		vector<Handle<GameObject>> _child_objects;
		vector<matrix_decomposition<real>> _bind_pose;
		
		// Precomputation step
		vector<mat4> pre_global_transforms{ mat4{} };
		// This is what we send to the graphics system.
		vector<mat4> final_bone_transforms{ mat4{} };

		bool _initialized = false;
		size_t _intialize_count = 0;
	};
}

