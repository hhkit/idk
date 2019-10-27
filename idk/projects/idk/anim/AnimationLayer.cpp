#include "stdafx.h"
#include "AnimationLayer.h"

namespace idk
{
	void AnimationLayer::Play(string_view animation_name, float offset)
	{
		is_playing = true;
		if (curr_state == animation_name)
			return;

		// cap at 1.0f
		normalized_time = std::max(std::min(offset, 1.0f), 0.0f);
		weight = default_weight;
		curr_state = animation_name;
		is_blending = false;
	}

	void AnimationLayer::Stop()
	{
		normalized_time = 0.0f;
		total_time = 0.0f;
		blend_time = 0.0f;
		blend_duration = 0.0f;
		blend_elapsed = 0.0f;
		weight = default_weight;
		is_stopping = true;
	}

	void AnimationLayer::Pause()
	{
		is_playing = false;
	}

	void AnimationLayer::Resume()
	{
		is_playing = true;
	}

	void AnimationLayer::BlendTo(string_view anim_name, float time)
	{
		blend_state = anim_name;
		blend_duration = time;
		blend_time = 0.0f;
		blend_elapsed = 0.0f;
		is_blending = true;
	}

	bool AnimationLayer::IsPlaying(string_view anim_name) const
	{
		return is_playing && anim_name == curr_state;
	}

	void AnimationLayer::Reset()
	{
		normalized_time = 0.0f;
		total_time = 0.0f;
		blend_time = 0.0f;
		blend_elapsed = 0.0f;
		weight = default_weight;
		
		is_playing = false;
		is_stopping = false;
		is_blending = false;
		// preview_playback = false;
		
		blend_state = string{};
		curr_state = default_state;
	}
}
