#pragma once

#include <core/ISystem.h>
#include <ds/span.h>

namespace idk
{
	enum class GraphicsAPI
	{
		OpenGL,
		//	DirectX,
		Vulkan,
		Default = OpenGL
	};

	class GraphicsSystem
		: public ISystem
	{
	public:
		void Init() override = 0;
		void Shutdown() override = 0;
		virtual void BufferGraphicsState(span<class MeshRenderer>, span<const class Transform>, span<const class Parent>) = 0;
		virtual void RenderBuffer() = 0;
		virtual GraphicsAPI GetAPI() = 0;
	};
}