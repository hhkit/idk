#pragma once
#include <vkn/BaseRenderPass.h>

#include <vkn/FrameGraphBuilder.h>
#include <vkn/FrameGraph.h>

namespace idk::vkn
{
	struct RenderObject;
	struct RenderState
	{

	};
	struct RenderUtils
	{
		static void DrawRenderObject(const RenderObject& ro, FrameGraphDetail::Context_t context)
		{
			for (auto& ub : ro.uniforms.ubos)
			{
				context.BindUniform(ub.name, ub.data);
			}
			for (auto& tex : ro.uniforms.tex)
			{
				context.BindUniform(tex.name, tex.data);
			}
			for (auto& vb : ro.vbos)
				context.BindVertexBuffer(vb.binding, vb.buffer, vb.offset);

			auto num_inst = ro.num_instances;
			auto first_vert = ro.first_vertex;
			auto first_inst = ro.first_instance;

			if (ro.is_index)
			{
				context.BindIndexBuffer(ro.index_buffer, ro.index_offset, ro.index_type);
				context.DrawIndexed(ro.num_indices, num_inst, first_vert, ro.first_index, first_inst);
			}
			else
				context.Draw(ro.num_vertices, num_inst, first_vert, first_inst);
		}
	};
	struct FrameGraphDemoPass : BaseRenderPass
	{
		struct RenderData
		{
			RenderState* rs;
			RenderTarget rt;
		};
		RenderData rd;
		FrameGraphResourceMutable gbuffer;
		FrameGraphResourceMutable depth_buffer;
		FrameGraphDemoPass(FrameGraphBuilder& builder, RenderData&& render_data) :
			rd{ std::move(render_data) },
			gbuffer{ builder.write(builder.CreateTexture(AttachmentDescription{"gbuffer",ivec2{1280,720}})) },
			depth_buffer{ builder.write(builder.CreateTexture(AttachmentDescription{"depth_buffer",ivec2{1280,720}})) }
		{
		}
		void Execute(FrameGraphDetail::Context_t context) override
		{
			auto& rs = *rd.rs;
			for (auto& ro : rs.render_objects) //render objects should be flattened as much as possible
			{
				for (auto& shader : ro.shaders)
				{
					context.BindShader(shader);
				}
				OverrideShaders(ro.shader, context);
				BindExtraUniforms(ro, context);
				RenderUtils::DrawRenderObject(ro, context);
			}
		}
		void OverrideShaders(span<ShaderModule> shader_modules, FrameGraphDetail::Context_t context)
		{
			//bind the appropriate shaders to use instead
		}
		void BindExtraUniforms(const RenderObject& ro, FrameGraphDetail::Context_t context)
		{
			//bind the extra uniforms according to the shaders here.
		}
	};
	struct RenderTarget
	{
		FrameGraphResource color;
		FrameGraphResource depth;
	};
	struct FrameGraphDemoFinish : BaseRenderPass
	{
		struct RenderData
		{
			RenderState* rs;
			RenderTarget render_target;
		};
		const FrameGraphDemoPass& gbuffer_pass;
		RenderData rd;
		FrameGraphResourceReadOnly gbuffer;
		FrameGraphResourceReadOnly depth_buffer;
		FrameGraphResourceMutable  draw_buffer;
		FrameGraphResourceMutable  draw_depth_buffer;
		FrameGraphDemoFinish(FrameGraphBuilder& builder, const FrameGraphDemoPass& gpass, RenderData&& render_data) :
			gbuffer_pass{ gpass },
			rd{ std::move(render_data) },
			gbuffer{ builder.read(gbuffer_pass.gbuffer) },
			depth_buffer{ builder.read(gbuffer_pass.depth_buffer) },
			draw_buffer{ builder.write(rd.render_target.color) },
			draw_depth_buffer{ builder.write(rd.render_target.depth)}
		{
		}
		void Execute(FrameGraphDetail::Context_t context) override
		{
			BindFinishPassShaders(context);
			DrawQuad(context);
		}
	};

	struct FrameGraphDemoTransparencyPass : BaseRenderPass
	{
		struct RenderData
		{
			RenderState* rs;
			RenderTarget draw_target;
		};
		RenderData rd;
		FrameGraphResourceMutable  draw_buffer;
		FrameGraphResourceMutable  draw_depth_buffer;
		FrameGraphDemoTransparencyPass(FrameGraphBuilder& builder , RenderData&& render_data) :
			rd{ std::move(render_data) },
			draw_buffer{ builder.write(rd.draw_target.color,WriteOptions{.clear = false}) },
			draw_depth_buffer{ builder.write(rd.draw_target.depth,WriteOptions{.clear=false}) }
		{
		}
		void Execute(FrameGraphDetail::Context_t context) override;

	};

	struct DemoRenderer
	{

		void Render(RenderState* rs, FrameGraph& fgraph)
		{
			RenderTarget my_target;

			auto& demo_pass = fgraph.addRenderPass<FrameGraphDemoPass>("Demo pass", FrameGraphDemoPass::RenderData{rs});
			auto& finish_pass = fgraph.addRenderPass<FrameGraphDemoFinish>("Demo pass Finish", FrameGraphDemoFinish::RenderData{ rs,my_target});
			my_target.color = finish_pass.draw_buffer;
			my_target.depth = finish_pass.depth_buffer;
			auto& transparency_pass = fgraph.addRenderPass<FrameGraphDemoTransparencyPass>("Demo pass Transparency", FrameGraphDemoTransparencyPass::RenderData{ rs,my_target });

		}
	};

}