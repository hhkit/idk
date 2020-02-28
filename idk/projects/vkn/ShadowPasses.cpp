#include "pch.h"
#include "ShadowPasses.h"
#include <vkn/GraphicsState.h>

#include <vkn/utils/utils.inl>

#include <vkn/FrameGraph.h>

#include <vkn/DrawSet.h>
#include <vkn/RenderBindings.inl>
#include <vkn/ShadowBindings.h>

#include <vkn/DrawSetRenderPass.h>



namespace idk::vkn
{
	constexpr int kPointLight = meta::IndexOf<LightVariant, PointLight>::value;
	constexpr int kDirectionalLight = meta::IndexOf<LightVariant, DirectionalLight>::value;
	constexpr int kSpotLight = meta::IndexOf<LightVariant, SpotLight>::value;

}


namespace idk::vkn::renderpasses
{
	namespace DirectionalShadow
	{
		using Bindings = bindings::ShadowBinding;
		using InstDrawSetBinding = GenericDrawSet<Bindings, InstMeshDrawSet>;
		using SkinnedInstDrawSetBinding = GenericDrawSet<Bindings,SkinnedMeshDrawSet>;

		using DrawSetBinding = CombinedMeshDrawSet<InstDrawSetBinding,SkinnedInstDrawSetBinding>;


		class RenderPass : public DrawSetRenderPass
		{
		public:
			RenderPass(FrameGraphBuilder& builder,RscHandle<FrameBuffer> target);
			void Execute(Context_t& context, BaseDrawSet& draw_set) override
			{
				context.DebugLabel(RenderTask::LabelLevel::eWhole, name);
				draw_set.Render(context);
			}
		private:
		};

	}










	struct Test
	{
		template<typename Binder, typename ...Args, typename = decltype(std::declval<Binder>().SetState(std::declval<Args&&>()...)) >
		void operator()(Binder&& binder, Args&&... args)
		{
			binder.SetState(std::forward<Args>(args)...);
		}

		template<typename ...Args>
		void operator()(Args&&... args){}

	};

	static const mat4 clip_mat = mat4{ vec4{1,0,0,0},vec4{0,1,0,0},vec4{0,0,0.5f,0},vec4{0,0,0.5f,1} };
	void AddDirectionalShadowPass(FrameGraph& frame_graph, GraphicsSystem::LightRenderRange shadow_range, const PreRenderData& state)
	{
		auto& light = state.shared_gfx_state->Lights()[shadow_range.light_index];
		auto& elem = light.light_maps[shadow_range.light_map_index];
		auto cam = CameraData{ Handle<GameObject>{}, light.shadow_layers, light.v, clip_mat * elem.cascade_projection };
		index_span inst_span{ shadow_range.inst_mesh_render_begin, shadow_range.inst_mesh_render_end };
		DirectionalShadow::Bindings bindings;
		bindings.for_each_binder<>(
			Test{},
			cam,
				*state.skeleton_transforms);
		DirectionalShadow::DrawSetBinding derp{ 
			{
				DirectionalShadow::InstDrawSetBinding{bindings,InstMeshDrawSet{inst_span.to_span(*state.shared_gfx_state->instanced_ros),state.shared_gfx_state->inst_mesh_render_buffer.buffer()}},
				DirectionalShadow::SkinnedInstDrawSetBinding{bindings,SkinnedMeshDrawSet{state.skinned_mesh_render}}
			}
		};
		frame_graph.addRenderPass<PassSetPair<DirectionalShadow::RenderPass, DirectionalShadow::DrawSetBinding>>("directional light", std::move(derp), elem.light_map);
		/*
		static const mat4 clip_mat = mat4{ vec4{1,0,0,0},vec4{0,1,0,0},vec4{0,0,0.5f,0},vec4{0,0,0.5f,1} };
		//for (auto& e : *state.d_lightmaps)
		{
			//for (auto& elem : light.light_maps)
			{
				auto cam = CameraData{ Handle<GameObject>{}, light.shadow_layers, light.v, clip_mat * elem.cascade_projection };
				ShadowBinding shadow_binding;
				shadow_binding.for_each_binder<has_setstate>(
					[](auto& binder, const CameraData& cam, const vector<SkeletonTransforms>& skel)
					{
						binder.SetState(cam, skel);
					},
					cam,
						*state.skeleton_transforms);
				GraphicsStateInterface gsi = { state };
				gsi.range = shadow_range;
				auto the_interface = vkn::ProcessRoUniforms(gsi, rs.ubo_manager, shadow_binding);
				the_interface.GenerateDS(rs.dpools, false);

				//auto& swapchain = view.Swapchain();


				//auto lm = elem.light_map->DepthAttachment().buffer;
				auto sz = elem.light_map->DepthAttachment().buffer->Size();

				vk::Rect2D render_area
				{
					vk::Offset2D{},
					vk::Extent2D{sz.x,sz.y}
				};
				auto& rt = elem.light_map.as<VknFrameBuffer>();
				vk::Framebuffer fb = rt.GetFramebuffer();
				auto  rp = rt.GetRenderPass();
				rt.PrepareDraw(cmd_buffer);
				vector<vec4> clear_colors
				{
					vec4{1}
				};
				//if (the_interface.DrawCalls().size())
				rs.FlagRendered();
				dbg::BeginLabel(cmd_buffer, "directional shadow", color{ 0,0.3f,0.3f,1 });
				RenderPipelineThingy(*state.shared_gfx_state, the_interface, GetPipelineManager(), cmd_buffer, clear_colors, fb, rp, true, render_area, render_area, frame_index);
				dbg::EndLabel(cmd_buffer);
				cmd_buffer.endRenderPass();
			}
			//cmd_buffer.end();
			//rs.ubo_manager.UpdateAllBuffers();
	
		}		
		*/
	}
	void AddSpotlightShadowPass  (FrameGraph& frame_graph, GraphicsSystem::LightRenderRange shadow_range, const PreRenderData& state);
	void AddPointShadowPass      (FrameGraph& frame_graph, GraphicsSystem::LightRenderRange shadow_range, const PreRenderData& state);

	void AddShadowPass(FrameGraph& frame_graph, GraphicsSystem::LightRenderRange shadow_range, const PreRenderData& state)
	{
		auto& light = state.shared_gfx_state->Lights()[shadow_range.light_index];
		switch (light.index)
		{
		case kPointLight:
			AddPointShadowPass(frame_graph, shadow_range, state);
			break;
		case kDirectionalLight:
			AddDirectionalShadowPass(frame_graph, shadow_range, state);
			break;
		case kSpotLight:
			AddSpotlightShadowPass(frame_graph, shadow_range, state);
			break;

		}
	}

}