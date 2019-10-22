#pragma once

#include <core/ISystem.h>
#include <idk.h>

#include "Animator.h"

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

	private:
		using BonePose = matrix_decomposition<real>;
		bool _was_paused = true;
		float _blend = 0.0f;

		// Animation passes: Animate -> Blend -> Layers(merging all layers) -> Finalize
		void AnimationPass(Animator& animator, AnimationLayer& layer);
		BonePose BlendPose(const BonePose& from, const BonePose& to, float delta);
		void LayersPass(Animator& animator);
		void FinalPass(Animator& animator, AnimationLayer& layer);
		void InterpolateBone(const anim::AnimatedBone& animated_bone, float time_in_ticks, matrix_decomposition<real>& curr_pose);

		
	};
}