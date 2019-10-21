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
		normalized_time = std::max(offset, 1.0f);
		curr_state = animation_name;
	}

	void AnimationLayer::Stop()
	{
		normalized_time = 0.0f;
		total_time = 0.0f;
		blend_time = 0.0f;
		is_stopping = true;
	}

	void AnimationLayer::Pause()
	{
		is_playing = false;
		preview_playback = false;
	}

	bool AnimationLayer::IsPlaying(string_view name) const
	{
		return is_playing && name == curr_state;
	}

	void AnimationLayer::Reset()
	{
		normalized_time = 0.0f;
		total_time = 0.0f;
		blend_time = 0.0f;
		weight = default_weight;
		
		is_playing = false;
		is_stopping = false;
		preview_playback = false;
		
		blend_state = string{};
		curr_state = default_state;
	}
}
