#include "stdafx.h"

namespace idk {
	BasicAnimationState* AnimationState::GetBasicState()
	{
		return std::get_if<BasicAnimationState>(&state_data);
	}
	BlendTree* AnimationState::GetBlendTree()
	{
		return std::get_if<BlendTree>(&state_data);
	}
	void AnimationState::AddTransition(size_t from, size_t to)
	{
		AnimationTransition new_transition;
		new_transition.transition_from_index = from;
		new_transition.transition_to_index = to;
		transitions.push_back(new_transition);
	}

	void AnimationState::RemoveTransition(size_t index)
	{
		if (index != 0 && index < transitions.size())
			transitions.erase(transitions.begin() + index);
	}

	AnimationTransition& AnimationState::GetTransition(size_t index)
	{
		if (index >= transitions.size())
			return transitions[0];

		return transitions[index];
	}
	bool AnimationState::IsBlendTree() const
	{
		return std::get_if<BlendTree>(&state_data) != nullptr;
	}
	void AnimationLayerState::Reset()
	{
		index = 0;
		is_playing = false;
		is_stopping = false;
		normalized_time = 0.0f;
		elapsed_time = 0.0f;
	}
}
