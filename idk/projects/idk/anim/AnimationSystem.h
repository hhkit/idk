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

	};
}