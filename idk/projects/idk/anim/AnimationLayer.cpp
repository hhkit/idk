#include "stdafx.h"
#include "AnimationLayer.h"
#include "AnimationSystem.h"

namespace idk
{
	bool AnimationLayer::Play(size_t index, float offset)
	{
		// Reset the blend/interrupt state and the playing state
		Reset();
		if (index >= anim_states.size())
		{
			LOG_CRASH_TO(LogPool::ANIM, "Animation States table is messed up. Somehow we are accessing out of bounds.");
			return false;
		}

		curr_state.is_playing = true;
		if (curr_state.index == index)
			return false;

		curr_state.index = index;
		// cap at 1.0f
		curr_state.normalized_time = std::max(std::min(offset, 1.0f), 0.0f);
		return true;
	}

	bool AnimationLayer::Play(string_view animation_name, float offset)
	{
		// Reset the blend/interrupt state and the playing state
		Reset();

		const auto found_anim = anim_state_table.find(animation_name.data());
		if (found_anim == anim_state_table.end())
		{
			return false;
		}

		return Play(found_anim->second, offset);
	}

	bool AnimationLayer::BlendTo(size_t index, float blend_time)
	{
		if (IsBlending() && !blend_interruptible)
			return false;

		if (blend_time < 0.00001f)
		{
			return Play(index);
		}

		if (index >= anim_states.size())
		{
			LOG_CRASH_TO(LogPool::ANIM, "Animation States table is messed up. Somehow we are accessing out of bounds.");
			return false;
		}

		// Ignore calls to blend to the current blend state
		if (blend_state.index == index)
		{
			return false;
		}

		// If i'm already blending and BlendTo was not called this frame, the blending was interuppted
		if (blend_state.is_playing)
		{
			if (!blend_this_frame)
			{
				blend_interrupt = true;
				blend_source = prev_poses;
			}
		}
		// If i'm not blending and the current animation is the same as the requested blend state, we do nothing. 
		else if (curr_state.index == index)
			return false;

		blend_state.index = index;
		blend_state.normalized_time = 0.0f;
		blend_state.is_playing = true;
		blend_this_frame = true;
		blend_duration = blend_time;
		blend_interruptible = false;
		return true;
	}

	bool AnimationLayer::BlendTo(string_view anim_name, float blend_time)
	{
		auto found_anim = anim_state_table.find(anim_name.data());
		if (found_anim == anim_state_table.end())
		{
			blend_state.Reset();
			blend_duration = 0.0f;
			blend_this_frame = false;
			blend_interrupt = false;

			LOG_WARNING_TO(LogPool::ANIM, "Unable to find animation state (" + string{ anim_name } +") to blend to.");
			return false;
		}

		return BlendTo(found_anim->second, blend_time);
		
	}

	void AnimationLayer::Stop()
	{
		curr_state.normalized_time = 0.0f;
		curr_state.is_stopping = true;
		
		blend_state.normalized_time = 0.0f;
		blend_state.is_stopping = true;
		blend_duration = 0.0f;

		blend_this_frame = false;
		blend_interrupt = false;

		playing_before_pause = false;
		blending_before_pause = false;
	}

	void AnimationLayer::Pause()
	{
		playing_before_pause = curr_state.is_playing;
		curr_state.is_playing = false;
		blending_before_pause = blend_state.is_playing;
		blend_state.is_playing = false;
	}

	void AnimationLayer::Resume()
	{
	
		curr_state.is_playing = playing_before_pause;
		blend_state.is_playing	= blending_before_pause;
	}

	string AnimationLayer::DefaultStateName() const
	{
		return anim_states[default_index].name;
	}

	string AnimationLayer::CurrentStateName() const
	{
		return anim_states[curr_state.index].name;
	}

	string AnimationLayer::BlendStateName() const
	{
		return anim_states[blend_state.index].name;
	}

	AnimationState& AnimationLayer::GetAnimationState(string_view anim_name)
	{
		auto res = anim_state_table.find(anim_name.data());
		if (res != anim_state_table.end())
			return anim_states[res->second];

		// index 0 is the null state
		return anim_states[0];
	}

	const AnimationState& AnimationLayer::GetAnimationState(string_view anim_name) const
	{
		auto res = anim_state_table.find(anim_name.data());
		if (res != anim_state_table.end())
			return GetAnimationState(res->second);

		// index 0 is the null state
		return anim_states[0];
	}

	AnimationState& AnimationLayer::GetAnimationState(size_t index)
	{
		if(index >= anim_states.size())
			return anim_states[0];
		return anim_states[index];
	}

	const AnimationState& AnimationLayer::GetAnimationState(size_t index) const
	{
		if (index >= anim_states.size())
			return anim_states[0];
		return anim_states[index];
	}

	void AnimationLayer::AddAnimation(RscHandle<anim::Animation> anim_rsc)
	{
		string anim_name = anim_rsc ? anim_rsc->Name().data() : "New State" ;
		// Check if name exists
		if (anim_state_table.find(anim_name) != anim_state_table.end())
		{
			string append = " 0";
			int count = 1;
			while (anim_state_table.find(anim_name + append) != anim_state_table.end())
			{
				// Generate a unique name
				append = " " + std::to_string(count++);
			}
			anim_name += append;
		}
		AnimationState state{ anim_name, true };
		state.state_data = variant<BasicAnimationState, BlendTree>{ BasicAnimationState{ anim_rsc } };
		anim_state_table.emplace(anim_name, anim_states.size());
		anim_states.emplace_back(state);
	}

	bool AnimationLayer::RenameAnimation(string_view from, string_view to)
	{
		string from_str{ from };
		string to_str{ to };
		auto res = anim_state_table.find(from_str);
		if (res == anim_state_table.end())
		{
			LOG_TO(LogPool::ANIM, string{ "Cannot rename animation state (" } +from.data() + ") to (" + to.data() + ").");
			return false;
		}

		auto found_dest = anim_state_table.find(to_str);
		if (found_dest != anim_state_table.end())
		{
			LOG_TO(LogPool::ANIM, string{ "Cannot rename animation state (" } + from.data() + ") to (" + to.data() + ").");
			return false;
		}


		auto copy = res->second;

		anim_state_table.erase(res);
		anim_state_table.emplace(to_str, copy);

		anim_states[copy].name = to_str;

		return true;
	}

	void AnimationLayer::RemoveAnimation(string_view anim_name)
	{
		string name_str{ anim_name };
		const auto found_clip = anim_state_table.find(anim_name.data());
		if (found_clip == anim_state_table.end() || found_clip->second >= anim_states.size())
			return;

		if (default_index == found_clip->second)
			default_index = 0;
		if (curr_state.index == found_clip->second)
			curr_state.Reset();
		if (blend_state.index == found_clip->second)
			ResetToDefault();

		const size_t removed_index = found_clip->second;
		anim_states.erase(anim_states.begin() + removed_index);
		anim_state_table.erase(found_clip);

		const size_t num_states = anim_states.size();
		for (size_t i = 1; i < num_states; ++i)
		{
			auto& anim_state = anim_states[i];
			const auto found = anim_state_table.find(anim_state.name);
			found->second = i;

			// Check all transitions for the removed index
			for (size_t k = 1; k < anim_state.transitions.size(); ++k)
			{
				auto& curr_transition = anim_state.transitions[k];

				// Set the transition to index to 0 if the state it is supposed to transition to is the one removed.
				if (curr_transition.transition_to_index == removed_index)
					curr_transition.transition_to_index = 0;
			}
		}
	}

	void AnimationLayer::RemoveAnimation(size_t index)
	{
		auto state = anim_states.begin() + index;
		RemoveAnimation(state->name);
	}

	bool AnimationLayer::IsPlaying() const
	{
		return blend_state.is_playing || curr_state.is_playing;
	}

	bool AnimationLayer::IsBlending() const
	{
		return blend_state.is_playing;
	}

	bool AnimationLayer::HasCurrAnimEnded() const
	{
		return curr_state.normalized_time >= 1.0f;
	}

	bool AnimationLayer::HasState(string_view anim_name) const
	{
		return anim_state_table.find(anim_name.data()) != anim_state_table.end();
	}

	void AnimationLayer::ResetToDefault()
	{
		Reset();
		curr_state.index = default_index;
		weight = default_weight;

	}

	void AnimationLayer::Reset()
	{
		curr_state.Reset();
		ResetBlend();
	}
	void AnimationLayer::ResetBlend()
	{
		blend_state.Reset();

		blend_duration = 0.0f;

		blend_this_frame = false;
		blend_interrupt = false;
		playing_before_pause = false;
		blending_before_pause = false;
		blend_interruptible = false;
	}
}
