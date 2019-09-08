#include "stdafx.h"
#include "GraphicsSystem.h"
#include <gfx/MeshRenderer.h>
#include <gfx/RenderObject.h>
namespace idk
{
	void GraphicsSystem::BufferGraphicsState(span<MeshRenderer> mesh_renderers, span<const class Transform>, span<const Camera> cameras)
	{
		if (!mesh_render_config)
		{
			mesh_render_config = std::make_shared<pipeline_config>();
			mesh_render_config->fill_type = FillType::eFill;
			mesh_render_config->prim_top = PrimitiveTopology::eTriangleList;
		}
		// todo: scenegraph traversal
		RenderBuffer result{};
		result.camera.reserve(cameras.size());

		for (auto& camera : cameras)
			result.camera.emplace_back(camera.GenerateCameraData());

		for (auto& elem : mesh_renderers)
			if (elem.IsActiveAndEnabled())
				result.mesh_render.emplace_back(elem.GenerateRenderObject()).config = mesh_render_config;

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