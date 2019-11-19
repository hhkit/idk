#pragma once
#include <idk.h>
#include "AnimationUtils.h"

namespace idk
{
	
	
	struct AnimationCondition
	{
		string param_name;
		anim::AnimDataType type = anim::AnimDataType::NONE;
		
		anim::ConditionIndex op_index = 0;
		float val_f;
		int val_i;
		bool val_b;
		bool val_t;	// trigger
	};

	struct AnimationTransition
	{
		bool valid = true;
		size_t transition_from_index = 0;
		size_t transition_to_index = 0;
		
		bool interruptible = false;
		bool has_exit_time = true;			// Evaluate no matter what if it is checked
		float exit_time = 0.8f;				// Evaluated only if has_exit_time == true
		float transition_offset = 0.0f;		// Offset the starting point of the state to blend to
		float transition_duration = 0.2f;	// How much of the transition state to play before it is fully weighted

		// All conditions must be met before the transition occurs. This includes exit time if applicable.
		vector<AnimationCondition> conditions;
	};
}