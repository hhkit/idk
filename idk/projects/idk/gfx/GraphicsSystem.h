#pragma once
#include <idk.h>
#include <core/ISystem.h>
#include <ds/span.h>
#include <gfx/RenderObject.h>

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
		//Converts the Renderers and transforms stores the render data into a buffer
		virtual void BufferGraphicsState(span<class MeshRenderer>, span<const class Transform>, span<const class Parent>);
		virtual void RenderRenderBuffer() = 0;
		virtual GraphicsAPI GetAPI() = 0;
	protected:
		struct RenderBuffer
		{
			vector<RenderObject> mesh_render;
			vector<RenderObject> skinned_mesh_render;
		};
		// triple buffered render state
		array<RenderBuffer, 3> object_buffer;
		unsigned               curr_write_buffer = 0;
		unsigned               curr_draw_buffer = 1;
		bool                   write_buffer_dirty = false;
	private:
		void SwapWritingBuffer();
		void SubmitBuffers(RenderBuffer&& buffer);

	};
}