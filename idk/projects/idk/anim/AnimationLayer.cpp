#include "stdafx.h"
#include "AnimationLayer.h"
#include "AnimationSystem.h"
#include <res/ResourceHandle.inl>
namespace idk
{
	bool AnimationLayer::Play(size_t index, float offset)
	{	
		if (index >= anim_states.size())
		{
			Reset();
			LOG_CRASH_TO(LogPool::ANIM, "Animation States table is messed up. Somehow we are accessing out of bounds.");
			return false;
		}

		if (IsInTransition() && !IsTransitionInterruptible())
			return false;
		
		// Reset the blend/interrupt state and the playing state
		const bool is_playing = curr_state.is_playing && !curr_state.is_stopping;
		if (is_playing && curr_state.index == index)
			return false;

		Reset();
		curr_state.is_playing = true;
		curr_state.index = index;
		// cap at 1.0f
		if (offset > 0.0f)
			curr_state.normalized_time = std::max(std::min(offset, 1.0f), 0.0f);
		else
			curr_state.normalized_time = anim_states[index].speed >= 0.0f ? 0.0f : 1.0f;
		return true;
	}

	bool AnimationLayer::Play(string_view anim_name, float offset)
	{
		auto found_anim = anim_name.empty() ? default_index : FindAnimationIndex(anim_name);
		if (found_anim >= anim_states.size())
		{
			Reset();
			return false;
		}

		return Play(found_anim, offset);
	}

	bool AnimationLayer::BlendTo(size_t index, float blend_time)
	{
		if (IsInTransition() && !IsTransitionInterruptible())
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
		blend_state.normalized_time = anim_states[index].speed < 0.0f ? 1.0f : 0.0f;
		blend_state.elapsed_time = 0.0f;
		blend_state.is_playing = true;
		blend_this_frame = true;
		// Cap the loop time to 1.0f if this state does not loop
		blend_duration = anim_states[index].loop ? blend_time : std::min(blend_time, 1.0f);
		transition_index = 0;
		return true;
	}

	bool AnimationLayer::BlendTo(string_view anim_name, float blend_time)
	{
		auto found_anim = FindAnimationIndex(anim_name);
		if (found_anim >= anim_states.size())
		{
			blend_state.Reset();
			blend_duration = anim_states[found_anim].speed < 0.0f ? 1.0f : 0.0f;
			blend_this_frame = false;
			blend_interrupt = false;

			LOG_WARNING_TO(LogPool::ANIM, "Unable to find animation state (" + string{ anim_name } +") to blend to.");
			return false;
		}

		return BlendTo(found_anim, blend_time);
		
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

	size_t AnimationLayer::FindAnimationIndex(string_view layer_name) const
	{
		size_t index;
		for (index = 0; index < anim_states.size(); ++index)
			if (anim_states[index].name == layer_name)
				break;
		return index;
	}

	AnimationState& AnimationLayer::GetAnimationState(string_view anim_name)
	{
		auto res = FindAnimationIndex(anim_name);
		return GetAnimationState(res);
	}

	const AnimationState& AnimationLayer::GetAnimationState(string_view anim_name) const
	{
		auto res = FindAnimationIndex(anim_name);
		return GetAnimationState(res);
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
		string append = "";
		int count = 0;

		while (FindAnimationIndex(anim_name + append) < anim_states.size())
		{
			// Generate a unique name
			append = " " + std::to_string(count++);
		}
		anim_name += append;

		AnimationState state{ anim_name, true };
		state.state_data = variant<BasicAnimationState, BlendTree>{ BasicAnimationState{ anim_rsc } };
		anim_states.emplace_back(state);

		// [0] = null_state, [1] = new state <- set this to the def state
		if (anim_states.size() == 2)
		{
			default_index = anim_states.size() - 1;
			curr_state.index = default_index;
		}
	}

	bool AnimationLayer::RenameAnimation(string_view from, string_view to)
	{
		auto src_found = FindAnimationIndex(from);
		if (src_found >= anim_states.size())
		{
			LOG_TO(LogPool::ANIM, string{ "Cannot rename animation state (" } +from.data() + ") to (" + to.data() + ").");
			return false;
		}

		auto found_dest = FindAnimationIndex(to);
		if (found_dest < anim_states.size())
		{
			LOG_TO(LogPool::ANIM, string{ "Cannot rename animation state (" } + from.data() + ") to (" + to.data() + ").");
			return false;
		}

		anim_states[src_found].name = to;

		return true;
	}

	bool AnimationLayer::RemoveAnimation(string_view anim_name)
	{
		string name_str{ anim_name };
		const auto found_clip = FindAnimationIndex(anim_name);
		return RemoveAnimation(found_clip);
	}

	bool AnimationLayer::RemoveAnimation(size_t index)
	{
		if (index >= anim_states.size())
			return false;

		if (default_index == index)
			default_index = 0;
		if (curr_state.index == index)
			curr_state.Reset();
		if (blend_state.index == index)
			ResetToDefault();

		anim_states.erase(anim_states.begin() + index);

		const size_t num_states = anim_states.size();
		for (size_t i = 1; i < num_states; ++i)
		{
			auto& anim_state = anim_states[i];

			// Check all transitions for the removed index
			for (size_t k = 1; k < anim_state.transitions.size(); ++k)
			{
				auto& curr_transition = anim_state.transitions[k];

				// Set the transition to index to 0 if the state it is supposed to transition to is the one removed.
				if (curr_transition.transition_to_index == index)
					curr_transition.transition_to_index = 0;
			}
		}
		return true;
	}

	bool AnimationLayer::IsPlaying() const
	{
		return blend_state.is_playing || curr_state.is_playing;
	}

	bool AnimationLayer::IsBlending() const
	{
		return blend_state.is_playing;
	}

	bool AnimationLayer::IsInTransition() const
	{
		return IsBlending() && transition_index > 0;
	}

	bool AnimationLayer::IsTransitionInterruptible() const
	{
		if (!IsInTransition())
			return false;
		auto& anim_state = GetAnimationState(curr_state.index);
		if (anim_state.valid)
		{
			auto& transition = anim_state.transitions[transition_index];
			return transition.valid && transition.interruptible;
		}
		return false;
	}

	bool AnimationLayer::HasCurrAnimEnded() const
	{
		return curr_state.normalized_time >= 1.0f;
	}

	bool AnimationLayer::HasState(string_view anim_name) const
	{
		return FindAnimationIndex(anim_name) < anim_states.size();
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
		transition_index = 0;
	}
}
