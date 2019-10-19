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
		
		bool _was_paused = true;
		float _blend = 0.0f;

		template<typename T>
		size_t find_key(const vector<T>& vec, float ticks)
		{
			for (unsigned i = 0; i < vec.size(); ++i)
			{
				if (ticks < static_cast<float>(vec[i].time))
				{
					return i - 1;
				}
			}
			

			return vec.size() - 1;
		}

		// Animation passes: Animate -> Blend -> Finalize
		void AnimationPass(Animator& animators);
		void FinalPass(Animator& animators);
		void InterpolateBone(const anim::AnimatedBone& animated_bone, float time_in_ticks, matrix_decomposition<real>& curr_pose);

		
	};
}