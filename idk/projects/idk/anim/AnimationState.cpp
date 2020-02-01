#include "stdafx.h"
#include "AnimationState.h"
namespace idk {
	BasicAnimationState* AnimationState::GetBasicState()
	{
		return std::get_if<BasicAnimationState>(&state_data);
	}
	const BasicAnimationState* AnimationState::GetBasicState() const
	{
		return std::get_if<BasicAnimationState>(&state_data);
	}
	BlendTree* AnimationState::GetBlendTree()
	{
		return std::get_if<BlendTree>(&state_data);
	}
	void AnimationState::ConvertToBlendTree()
	{
		BlendTree new_tree{};
		new_tree.motions.emplace_back(BlendTreeMotion{ GetBasicState()->motion });
		state_data.emplace<1>(new_tree);
	}
	void AnimationState::AddTransition(size_t from, size_t to)
	{
		AnimationTransition new_transition;
		new_transition.transition_from_index = from;
		new_transition.transition_to_index = to;
		transitions.push_back(new_transition);
	}

	bool AnimationState::RemoveTransition(size_t index)
	{
		if (index != 0 && index < transitions.size())
		{
			transitions.erase(transitions.begin() + index);
			return true;
		}
		return false;
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
	void BlendTree::ComputeWeights(float param_val)
	{
		float sum = 0.0f;

		size_t motions_sz = motions.size();
		if (motions_sz <= 1)
		{
			for (size_t i = 0; i < motions_sz; ++i)
			{
				motions[i].weight = 1.0f;
			}
		}
		else
		{
			param_val = std::clamp(param_val, motions.front().thresholds[0], motions.back().thresholds[0]);
			for (size_t i = 0; i < motions_sz; ++i)
			{
				const float curr_threshold = motions[i].thresholds[0];
				float prev_threshold = curr_threshold;
				float next_threshold = curr_threshold;

				if (i != 0)
					prev_threshold = motions[i - 1].thresholds[0];

				if (i + 1 < motions_sz)
					next_threshold = motions[i + 1].thresholds[0];

				motions[i].weight = idk::anim::piecewise_linear(prev_threshold, curr_threshold, next_threshold, param_val);

				sum += motions[i].weight;
			}

			// Normalize the weights
			for (auto& blend_motion : motions)
				blend_motion.weight /= sum;
		}
	}
}
