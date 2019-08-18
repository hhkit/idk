#pragma once

#include <core/ISystem.h>

namespace idk
{
	class GraphicsSystem
		: public ISystem
	{
	public:
		void Init() override = 0;
		virtual void BeginFrame() = 0;
		virtual void Draw() = 0;
		virtual void EndFrame() = 0;
		void Shutdown() override = 0;
	};
}