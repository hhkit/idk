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
		string default_state{};
		float default_offset = 0.0f;
		float default_weight = 1.0f;

		std::array<bool, 100> bone_mask{ false };
		AnimLayerBlend blend_type = AnimLayerBlend::Override_Blend;

		// Run time values
		float weight = 1.0f;

		float normalized_time = 0.0f;
		float blend_time = 0.0f;
		float blend_duration = 0.0f;
		float blend_elapsed = 0.0f;
		float total_time = 0.0f;

		string curr_state{}, blend_state{};
		bool is_playing = false, is_stopping = false, is_blending = false;
		vector<matrix_decomposition<real>> prev_poses;

		void Play(string_view animation_name, float offset = 0.0f);
		// void BlendTo(string_view animation_name, float offset);
		void Stop();
		void Pause();
		void Resume();
		void BlendTo(string_view anim_name, float blend_time = 0.2f);
		
		bool IsPlaying(string_view name) const;

		void Reset();
	};
}