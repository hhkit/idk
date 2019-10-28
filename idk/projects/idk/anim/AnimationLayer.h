#pragma once
#include <idk.h>
#include <util/enum.h>
#include "AnimationState.h"
namespace idk
{
	ENUM (AnimLayerBlend, char,
		Base_Blend,
		Override_Blend,
		Additive_Blend
	);

	struct AnimationLayer
	{
		// Serialized data - Default values
		string name{};
		string default_state;
		float default_weight = 1.0f;

		std::array<bool, 100> bone_mask{ false };
		AnimLayerBlend blend_type = AnimLayerBlend::Override_Blend;

		// Run time values
		float weight = 1.0f;

		float blend_duration = 0.0f;
		AnimationLayerState curr_state, blend_state;

		bool blending_before_pause = false;
		bool blend_this_frame = false, blend_interrupt = false;

		vector<matrix_decomposition<real>> prev_poses;
		vector<matrix_decomposition<real>> blend_source;

		void Play(string_view animation_name, float offset = 0.0f);
		void BlendTo(string_view anim_name, float blend_time = 0.2f);
		void Stop();
		void Pause();
		void Resume();
		
		
		bool IsPlaying() const;

		void Reset();
	};
}