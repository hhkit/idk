#pragma once
#include <res/ResourceHandle.h>
#include "AnimationUtils.h"
namespace idk
{
	namespace anim { class Animation; }
	
	struct BasicAnimationState
	{
		RscHandle<anim::Animation> motion{};
	};

	struct BlendTreeMotion
	{
		string name{};
		RscHandle<anim::Animation> motion{};
		float data[2];
	};

	struct BlendTree
	{
		// set<BlendTreeMotion> motions{};
		std::array<string, 2> params;

		BlendTreeType blend_tree_type = BlendTreeType::BlendTree_1D;
	};

	struct AnimationState
	{
		string name{};
		
		bool valid = false;
		bool loop = true;
		float speed = 1.0f;

		variant<BasicAnimationState, BlendTree> state_data;

		BasicAnimationState* GetBasicState();
		BlendTree* GetBlendTree();
		bool IsBlendTree() const;
	};

	struct AnimationLayerState
	{
		string name{};

		bool is_playing = false, is_stopping = false;
		float normalized_time = 0.0f;
	};
	
}