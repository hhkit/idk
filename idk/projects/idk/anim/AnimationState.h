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
		set<BlendTreeMotion> motions{};
		string params[2]{};

		BlendTreeType blend_tree_type = BlendTreeType::BlendTree_1D;
	};

	struct AnimationState
	{
		string name{};
		
		bool enabled = false;
		bool loop = true;
		float speed = 1.0f;

		using StateData = variant<BasicAnimationState, BlendTree>;
		StateData state_data;

		BasicAnimationState* GetBasicState();
		BlendTree* GetBlendTree();
		bool IsBlendTree() const;
	};

	
}