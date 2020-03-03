#include "pch.h"
#include "MaskedPass.h"
#include <vkn/BaseRenderPass.h>
#include <vkn/DrawSetRenderPass.h>

#include <res/ResourceHandle.inl>

#include <vkn/FrameGraph.h>
#include <vkn/FrameGraphBuilder.h>

#include <vkn/DeferredPbrRoBind.h>

#include <vkn/GraphicsState.h>

namespace idk::vkn::renderpasses
{
	class MaskedRenderPass : public DrawSetRenderPass
	{
	public:
		FrameGraphResource color_rsc;
		FrameGraphResource depth_rsc;

		MaskedRenderPass(FrameGraphBuilder& builder,FrameGraphResource color_att, FrameGraphResource depth_att)
		{
			auto color = builder.write(color_att,WriteOptions{false});
			auto depth = builder.write(depth_att,WriteOptions{false});

			color_rsc = color;
			depth_rsc = depth;

			builder.set_output_attachment(color, 0,
				AttachmentDescription::make
				(
					vk::AttachmentLoadOp::eLoad,
					vk::AttachmentStoreOp::eStore,
					vk::AttachmentLoadOp::eDontCare,
					vk::AttachmentStoreOp::eDontCare,
					vk::ImageSubresourceRange
					{
						vk::ImageAspectFlagBits::eColor,0,1,0,1
					},
					{}, {}, {},vk::ImageLayout::eGeneral
				)
			);

			builder.set_depth_stencil_attachment(depth,
				AttachmentDescription::make
				(
					vk::AttachmentLoadOp::eLoad,
					vk::AttachmentStoreOp::eStore,
					vk::AttachmentLoadOp::eDontCare,
					vk::AttachmentStoreOp::eDontCare
				)
			);

		}
		virtual void Execute(Context_t& context, BaseDrawSet& draw_set)
		{
			context.DebugLabel(RenderTask::LabelLevel::eWhole, name);

			AttachmentBlendConfig blend{};
			blend.blend_enable = true;
			blend.dst_color_blend_factor = BlendFactor::eOneMinusSrcAlpha;
			blend.src_color_blend_factor = BlendFactor::eSrcAlpha ;
			blend.color_blend_op = BlendOp::eAdd;
			blend.alpha_blend_op = BlendOp::eAdd;
			blend.dst_alpha_blend_factor = BlendFactor::eOneMinusSrcAlpha;
			blend.src_alpha_blend_factor = BlendFactor::eOne;
			context.SetBlend(0,blend);
			draw_set.Render(context);
		}
	private:
	};

	using PbrInstDrawSet = GenericDrawSet<bindings::DeferredPbrRoBind, InstMeshDrawSet>;
	using PbrAniDrawSet = GenericDrawSet<bindings::DeferredPbrAniBind, SkinnedMeshDrawSet>;
	using PbrSet = CombinedMeshDrawSet<PbrAniDrawSet, PbrInstDrawSet>;

	bindings::DeferredPbrInfo MakePbrInfo(const GraphicsState& gfx_state)
	{
		bindings::StandardVertexBindings::StateInfo state;
		state.SetState(gfx_state);
		bindings::DeferredPbrInfo info{
			.viewport = gfx_state.camera.viewport,
			.blend = BlendMode::Opaque,
			.model = ShadingModel::DefaultLit,
			.material_instances = gfx_state.material_instances,
			.vertex_state_info = state,
		};
		return info;
	}

	PbrSet MakePbrSet(bindings::DeferredPbrInfo info, const GraphicsState& gfx_state)
	{
		return PbrSet{
			{
				PbrAniDrawSet{
						bindings::make_deferred_pbr_ani_bind(info),
						SkinnedMeshDrawSet{span{gfx_state.skinned_mesh_render}}
				},
				PbrInstDrawSet{bindings::make_deferred_pbr_ro_bind(info),
										InstMeshDrawSet{
											span{
												gfx_state.shared_gfx_state->instanced_ros->data() + gfx_state.range.inst_mesh_render_begin,gfx_state.shared_gfx_state->instanced_ros->data() + gfx_state.range.inst_mesh_render_end},
												gfx_state.shared_gfx_state->inst_mesh_render_buffer.buffer()
										}
					},
			}
		};
	}


	std::pair<FrameGraphResource, FrameGraphResource> AddTransparentPass(FrameGraph& graph, FrameGraphResource color_att, FrameGraphResource depth_att, const GraphicsState& gfx_state)
	{
		auto info = MakePbrInfo(gfx_state);
		info.blend = BlendMode::Masked;
		info.model = ShadingModel::Unlit;
		auto& tra_pass=graph.addRenderPass<PassSetPair<MaskedRenderPass,PbrSet>>("Transparent Stuff", MakePbrSet(info, gfx_state), color_att, depth_att).RenderPass();
		return {tra_pass.color_rsc,tra_pass.depth_rsc};
	}
}
