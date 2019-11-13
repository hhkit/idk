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
	}
}
