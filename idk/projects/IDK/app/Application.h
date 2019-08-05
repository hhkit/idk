#pragma once
#include <core/ISystem.h>

namespace idk
{
	class Application
		: public ISystem
	{
	public:
		virtual void PollEvents() = 0;
	};
}