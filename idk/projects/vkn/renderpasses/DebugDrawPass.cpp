#include "pch.h"
#include "DebugDrawPass.h"

#include <vkn/BaseRenderPass.h>

#include <gfx/GraphicsSystem.h>

#include <vkn/FrameGraphBuilder.h>
#include <vkn/FrameGraph.h>
#include <vkn/DrawSet.h>

namespace idk::vkn::renderpasses
{




	class DebugRenderPass : public BaseRenderPass
	{
	public:
		const GraphicsState& state;
		rect vp;
		FrameGraphResource color;
		FrameGraphResource depth;
		DebugRenderPass(FrameGraphBuilder& builder, FrameGraphResource col,FrameGraphResource dep, const GraphicsState& gfx_state, rect viewport) : state{gfx_state}, color{ col }, depth{dep},vp{viewport}
		{
			color = builder.write(color,WriteOptions{false});
			depth = builder.write(depth,WriteOptions{false});

			builder.set_output_attachment(color,0,
				AttachmentDescription::make(
					vk::AttachmentLoadOp::eLoad,
					vk::AttachmentStoreOp::eStore,
					vk::AttachmentLoadOp::eDontCare,
					vk::AttachmentStoreOp::eDontCare,
					{},
					{},
					{},
					{},
					vk::ImageLayout::eGeneral
				)
			);
			builder.set_depth_stencil_attachment(depth,
				AttachmentDescription::make(
					vk::AttachmentLoadOp::eLoad,
					vk::AttachmentStoreOp::eStore,
					vk::AttachmentLoadOp::eDontCare,
					vk::AttachmentStoreOp::eDontCare,
					{},
					{},
					{},
					{},
					vk::ImageLayout::eGeneral
				)
			);
		}
		void Execute(Context_t& context) override
		{
			context.DebugLabel(RenderTask::LabelLevel::eWhole, name);

			context.SetScissorsViewport(vp);
			for (auto& p_dc : state.dbg_render)
			{
				auto& dc = *p_dc;
				if (context.SetPipeline(*dc.pipeline))
				{
					FakeMat4 view_mat = state.camera.view_matrix;
					FakeMat4 proj_mat = mat4{ 1,0,0,0,   0,1,0,0,   0,0,0.5f,0.5f, 0,0,0,1 }*state.camera.projection_matrix;
					context.BindShader(ShaderStage::Vertex, Core::GetSystem<GraphicsSystem>().renderer_vertex_shaders[VDebug]);
					context.BindShader(ShaderStage::Fragment,Core::GetSystem<GraphicsSystem>().renderer_fragment_shaders[FDebug]);
					context.BindUniform("View", 0, hlp::to_data(view_mat));
					context.BindUniform("Proj", 0, hlp::to_data(proj_mat));
				}

				for (auto& [binding, buffer] : dc.mesh_buffer)
				{
					context.BindVertexBufferByBinding(binding, buffer.buffer, buffer.offset);
				}
				//cmd_buffer.bindVertexBuffers(0,
				//	{
				//		 *mesh.Get(attrib_index::Position).buffer(),//dc.mesh_buffer[DbgBufferType::ePerVtx].find(0)->second.buffer,
				//		dc.mesh_buffer[DbgBufferType::ePerInst].find(1)->second.buffer
				//	},
				//	{
				//		0,0
				//	}
				//	);
				//cmd_buffer.bindIndexBuffer(dc.index_buffer.buffer, 0, vk::IndexType::eUint16);
				//cmd_buffer.drawIndexed(mesh.IndexCount(), dc.nu, 0, 0, 0);
				if (dc.num_indices)
				{
					context.BindIndexBuffer(dc.index_buffer.buffer, dc.index_buffer.offset, vk::IndexType::eUint16);
					context.DrawIndexed(dc.num_indices, dc.num_instances, dc.index_buffer.offset, 0, 0);
				}
				else
					context.DrawIndexed(dc.num_vertices, dc.num_instances, 0, 0,0);

			}
		}
	private:
	};




	std::pair<FrameGraphResource, FrameGraphResource> AddDebugDrawPass(FrameGraph& graph, rect viewport, const GraphicsState& gfx_state, FrameGraphResource color, FrameGraphResource depth)
	{
		auto& rp = graph.addRenderPass<DebugRenderPass>("Debug Rendering",color,depth,gfx_state,viewport);
		return std::pair<FrameGraphResource, FrameGraphResource>(rp.color,rp.depth);
	}
}
