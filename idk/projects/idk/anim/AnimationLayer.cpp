#include "stdafx.h"
#include "AnimationLayer.h"

namespace idk
{
	void AnimationLayer::Play(string_view animation_name, float offset)
	{
		curr_state.is_playing = true;

		// cap at 1.0f
		curr_state.normalized_time = std::max(std::min(offset, 1.0f), 0.0f);
		if (curr_state.name == animation_name)
			return;

		
		curr_state.name = animation_name;

		// weight = default_weight;
		blend_state = AnimationLayerState{};
		blending_before_pause = false;
	}

	void AnimationLayer::Stop()
	{
		curr_state.normalized_time = 0.0f;
		curr_state.is_stopping = true;

		blend_state.normalized_time = 0.0f;
		blend_state.is_stopping = true;

		blend_duration = 0.0f;
		weight = default_weight;
		blending_before_pause = false;
	}

	void AnimationLayer::Pause()
	{
		curr_state.is_playing = false;
		blending_before_pause = blend_state.is_playing;
		blend_state.is_playing = false;
	}

	void AnimationLayer::Resume()
	{
		curr_state.is_playing	= false;
		blend_state.is_playing	= blending_before_pause;
	}

	void AnimationLayer::BlendTo(string_view anim_name, float time)
	{
		// Cannot blend to myself and cannot blend to the current state
		if (curr_state.name == anim_name || blend_state.name == anim_name)
		{
			return;
		}
			
		blend_state.name = anim_name;
		blend_state.normalized_time = 0.0f;
		blend_state.is_playing = true;
		blend_duration = time;
	}

	bool AnimationLayer::IsPlaying() const
	{
		return blend_state.is_playing || curr_state.is_playing;
	}

	void AnimationLayer::Reset()
	{
		curr_state.name = default_state;
		curr_state.is_playing = false;
		curr_state.is_stopping = false;
		curr_state.normalized_time = 0.0f;

		blend_state = AnimationLayerState{};
		blend_duration = 0.0f;
		weight = default_weight;
		
		
		blending_before_pause = false;
		// preview_playback = false;
	}
}
