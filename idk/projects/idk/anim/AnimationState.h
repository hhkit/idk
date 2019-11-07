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
		RscHandle<anim::Animation> motion{};
		std::array< float, 2> thresholds{0.0f, 0.0f};

		float speed = 1.0f;

		float weight = 1.0f;
	};

	struct BlendTree
	{
		vector<BlendTreeMotion> motions;
		std::array<string, 2> params;

		anim::BlendTreeType blend_tree_type = anim::BlendTreeType::BlendTree_1D;
		bool weights_cached = false;

		float def_data[2];
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