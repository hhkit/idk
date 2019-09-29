#pragma once

#include <core/ISystem.h>
#include <idk.h>

#include "AnimationController.h"

namespace idk
{
	class AnimationSystem
		: public ISystem
	{
	public:
		virtual void Init() override;

		void Update(span<AnimationController>);

		virtual void Shutdown() override;

	private:
		template<typename T>
		size_t find_key(const vector<T>& vec, float ticks)
		{
			for (unsigned i = 0; i < vec.size(); ++i)
			{
				if (ticks < static_cast<float>(vec[i]._time))
				{
					return i - 1;
				}
			}
			

			return vec.size() - 1;
		}

		anim::Skeleton::BonePose interpolateChannel(const anim::Animation::Channel& channel, float time_in_ticks);
	};
}