#pragma once

#include <core/ISystem.h>
#include <ds/span.h>

namespace idk
{
	class GraphicsSystem
		: public ISystem
	{
	public:
		void Init() override = 0;
		void Shutdown() override = 0;
		virtual void BufferGraphicsState(span<class MeshRenderer>, span<const class Transform>, span<const class Parent>) = 0;
		virtual void RenderBuffer() = 0;
	};
}