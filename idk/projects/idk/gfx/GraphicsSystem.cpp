#include "stdafx.h"
#include "GraphicsSystem.h"
#include <gfx/MeshRenderer.h>
#include <gfx/RenderObject.h>
namespace idk
{
	void GraphicsSystem::BufferGraphicsState(span<MeshRenderer> mesh_renderers, span<const class Transform>, span<const class Parent>)
	{
		// todo: scenegraph traversal
		RenderBuffer result{};
		std::vector<RenderObject>& objects = result.mesh_render;
		for (auto& elem : mesh_renderers)
			if (elem.IsActiveAndEnabled())
				objects.emplace_back(elem.GenerateRenderObject());
		SubmitBuffers(std::move(result));
	}

	void GraphicsSystem::SwapWritingBuffer()
	{
	}

	void GraphicsSystem::SubmitBuffers(RenderBuffer&& buffer)
	{
		object_buffer[curr_write_buffer] = std::move(buffer);
		SwapWritingBuffer();
	}


}