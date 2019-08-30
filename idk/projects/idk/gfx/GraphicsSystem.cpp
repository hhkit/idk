#include "stdafx.h"
#include "GraphicsSystem.h"
#include <gfx/MeshRenderer.h>
#include <gfx/RenderObject.h>
namespace idk
{
	void GraphicsSystem::BufferGraphicsState(span<MeshRenderer> mesh_renderers, span<const class Transform>, span<const class Parent>, span<const Camera> cameras)
	{
		// todo: scenegraph traversal
		RenderBuffer result{};
		result.states.reserve(cameras.size());
		//Temporarily commented out cause cameras aren't passed in yet.
		//for (auto& camera : cameras)
		{
			GraphicsState state{};
		//Temporarily commented out cause cameras aren't passed in yet.
		//	state.camera = camera;
			vector<RenderObject>& objects =state.mesh_render;
			for (auto& elem : mesh_renderers)
				if (elem.IsActiveAndEnabled())
					objects.emplace_back(elem.GenerateRenderObject());
			result.states.emplace_back(std::move(state));
		}
		SubmitBuffers(std::move(result));
	}

	void GraphicsSystem::SwapWritingBuffer()
	{
		//write_buffer_dirty = true;
		//tmp
		curr_draw_buffer= curr_write_buffer;
	}

	void GraphicsSystem::SubmitBuffers(RenderBuffer&& buffer)
	{
		object_buffer[curr_write_buffer] = std::move(buffer);
		SwapWritingBuffer();
	}


}