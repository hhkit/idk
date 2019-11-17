#pragma once
#include <res/ResourceHandle.h>
#include "AnimationTransition.h"
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
		vec3 node_position{};

		variant<BasicAnimationState, BlendTree> state_data;
		vector<AnimationTransition> transitions;

		BasicAnimationState* GetBasicState();
		BlendTree* GetBlendTree();
		void AddTransition(size_t from, size_t to);
		bool IsBlendTree() const;
	};

	struct AnimationLayerState
	{
		// string name{};
		size_t index = 0;

		bool is_playing = false, is_stopping = false;
		float normalized_time = 0.0f;
		float elapsed_time = 0.0f;
		void Reset();
	};
	
}