#pragma once
#include <idk.h>
#include <anim/AnimationState.h>
#include <math/matrix_decomposition.h>
#include <util/enum.h>

namespace idk
{
	ENUM (AnimLayerBlend, char,
		Override_Blend,
		Additive_Blend
	);

	struct AnimationLayer
	{
		// Serialized data - Default values
		string name{};

		size_t default_index = 0;
		AnimationLayerState curr_state, blend_state;
		size_t transition_index = 0;

		// Run time values
		// layer weight and blend type (currently only have override blend)
		float default_weight = 1.0f;
		float weight = 1.0f;
		AnimLayerBlend blend_type = AnimLayerBlend::Override_Blend;
		std::array<bool, 100> bone_mask{ false };

		// Controls blending to different states in run-time
		float blend_duration = 0.0f;
		bool playing_before_pause = false, blending_before_pause = false;
		bool blend_this_frame = false, blend_interrupt = false;

		// Caching
		vector<matrix_decomposition<real>> prev_poses;
		vector<matrix_decomposition<real>> blend_source;

		hash_table<string, size_t> anim_state_table;
		vector<AnimationState> anim_states{ AnimationState{} };

		bool Play(size_t index, float offset = 0.0f);
		bool Play(string_view animation_name, float offset = 0.0f);
		bool BlendTo(size_t index, float blend_time = 0.2f);
		bool BlendTo(string_view anim_name, float blend_time = 0.2f);
		void Stop();
		void Pause();
		void Resume();

		string DefaultStateName() const;
		string CurrentStateName() const;
		string BlendStateName() const;

		AnimationState& GetAnimationState(string_view name);
		const AnimationState& GetAnimationState(string_view name) const;

		AnimationState& GetAnimationState(size_t index);
		const AnimationState& GetAnimationState(size_t index) const;

		// Engine Setters
		void AddAnimation(RscHandle<anim::Animation> anim_rsc);
		bool RenameAnimation(string_view from, string_view to);
		void RemoveAnimation(string_view name);
		void RemoveAnimation(size_t index);

		bool IsPlaying() const;
		bool IsBlending() const;
		bool IsInTransition() const;
		bool IsTransitionInterruptible() const;
		bool HasCurrAnimEnded() const;
		bool HasState(string_view name) const;
		
		void Reset();
		void ResetBlend();
		void ResetToDefault();
	};
}