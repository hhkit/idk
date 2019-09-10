#include "stdafx.h"
#include "GraphicsSystem.h"
#include <gfx/MeshRenderer.h>
#include <gfx/RenderObject.h>
namespace idk
{
	void GraphicsSystem::BufferGraphicsState(
		span<MeshRenderer> mesh_renderers, 
		span<const class Transform>, 
		span<const Camera> cameras, 
		span<const Light> lights)
	{
		// todo: scenegraph traversal
		RenderBuffer result{};
		result.camera.reserve(cameras.size());

		// memcpy the lights until there is a smarter implementation
		result.lights.reserve(lights.size());
		for (auto& elem : lights)
			result.lights.emplace_back(elem.GenerateLightData());

		for (auto& camera : cameras)
			result.camera.emplace_back(camera.GenerateCameraData());

		for (auto& elem : mesh_renderers)
			if (elem.IsActiveAndEnabled())
				result.mesh_render.emplace_back(elem.GenerateRenderObject());

		SubmitBuffers(std::move(result));
	}

	void GraphicsSystem::SwapWritingBuffer()
	{
		write_buffer_dirty = true;
	}

	void GraphicsSystem::SubmitBuffers(RenderBuffer&& buffer)
	{
		object_buffer[curr_write_buffer] = std::move(buffer);
		SwapWritingBuffer();
	}


}