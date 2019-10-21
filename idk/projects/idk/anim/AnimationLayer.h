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
		float total_time = 0.0f;

		string curr_state{}, blend_state{};
		bool is_playing = false, is_stopping = false, preview_playback = false;

		void Play(string_view animation_name, float offset);
		// void BlendTo(string_view animation_name, float offset);
		void Stop();
		void Pause();

		bool IsPlaying(string_view name) const;

		void Reset();
	};
}