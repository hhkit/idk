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
	}

	void AnimationLayer::Stop()
	{
		normalized_time = 0.0f;
		total_time = 0.0f;
		blend_time = 0.0f;
		weight = default_weight;
		is_stopping = true;
	}

	void AnimationLayer::Pause()
	{
		is_playing = false;
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
		weight = default_weight;
		
		is_playing = false;
		is_stopping = false;
		// preview_playback = false;
		
		blend_state = string{};
		curr_state = default_state;
	}
}
