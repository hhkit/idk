#pragma once

#include <core/ISystem.h>
#include <idk.h>

#include "Animator.h"
#include <queue>

namespace idk
{
	class AnimationSystem
		: public ISystem
	{
	public:
		virtual void Init() override;

		void Update(span<Animator>);
		void UpdatePaused(span<Animator>);
		virtual void Shutdown() override;
		
		// Helper functions
		void GenerateSkeletonTree(Animator& animator);
		void SaveBindPose(Animator& animator);
		void RestoreBindPose(Animator& animator);
		void HardReset(Animator& animator);

		using BonePose = matrix_decomposition<real>;
	private:
		
		bool _was_paused = true;
		float _blend = 0.0f;

		
		BonePose ComputePose(Animator& animator, AnimationLayer& layer, AnimationLayerState& state, size_t bone_index);
		BonePose ComputeBlendTreePose(Animator& animator, AnimationLayer& layer, AnimationLayerState& state, size_t bone_index);

		//BonePose ComputeLastPose(Animator& animator, AnimationLayer& layer, AnimationLayerState& state, size_t bone_index);
		//BonePose ComputeLayerPose(Animator& animator, AnimationLayer& layer, size_t bone_index);
		BonePose AnimationPass(Animator& animator, AnimationLayer& layer, size_t bone_index);
		// BonePose BlendingPass(Animator& animator, AnimationLayer& layer, size_t bone_index);
		BonePose BlendPose(const BonePose& from, const BonePose& to, float delta);

		void EvaluateTransitions(Animator& animator, AnimationLayer& layer);
		size_t LayersPass(Animator& animator);
		void AdvanceLayers(Animator& animator);
		void AdvanceBlendTree(AnimationLayerState& layer_state, AnimationState& state) const;

		void FinalPass(Animator& animator);
		void InterpolateBone(const anim::AnimatedBone& animated_bone, float time_in_ticks, matrix_decomposition<real>& curr_pose);

		void InitializeAnimators(bool play);

		vector<Handle<Animator>> _creation_queue;
	};
}