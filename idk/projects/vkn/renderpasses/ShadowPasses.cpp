#include "pch.h"
#include "ShadowPasses.h"
#include <vkn/GraphicsState.h>

#include <vkn/utils/utils.inl>

#include <vkn/FrameGraph.h>

#include <res/ResourceHandle.inl>

#include <vkn/DrawSet.h>
#include <vkn/RenderBindings.inl>
#include <vkn/ShadowBindings.h>

#include <vkn/DrawSetRenderPass.h>

#include <meta/meta.inl>

#include <math/matrix_transforms.inl>
#include <vkn/binding_util.h>


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
			RenderPass(FrameGraphBuilder& builder, RscHandle<FrameBuffer> target)
			{
				uvec2 rt_size = target->Size();
				auto& depth_att = target->DepthAttachment();
				auto& depth_tex = depth_att.buffer.as<VknTexture>();
				;
				auto depth = builder.CreateTexture(TextureDescription::make
					(
						name + " depth",
						vk::ImageUsageFlagBits::eDepthStencilAttachment,
						RscHandle<VknTexture>{depth_att.buffer}
					));
				depth = builder.write(depth);
				
				builder.set_depth_stencil_attachment(depth, 
					AttachmentDescription::make(
						vk::AttachmentLoadOp::eClear,
						vk::AttachmentStoreOp::eStore,
						vk::AttachmentLoadOp::eDontCare,
						vk::AttachmentStoreOp::eDontCare,
						depth_tex.FullRange(),
						vk::ClearValue{ vk::ClearDepthStencilValue{1.0f,0} },
						{},
						{},
						vk::ImageLayout::eGeneral
					)
				);
			}
			void Execute(Context_t& context, BaseDrawSet& draw_set) override
			{
				context.DebugLabel(RenderTask::LabelLevel::eWhole, name);
				draw_set.Render(context);
			}
		private:
		};

	}
#pragma optimize("",off)
	namespace PointShadow
	{
		using Bindings = bindings::PointShadowBinding;
		using InstDrawSetBinding = GenericDrawSet<Bindings, InstMeshDrawSet>;
		using SkinnedInstDrawSetBinding = GenericDrawSet<Bindings, SkinnedMeshDrawSet>;
		
		using DrawSetBinding = CombinedMeshDrawSet<InstDrawSetBinding, SkinnedInstDrawSetBinding>;


		class RenderPass : public DrawSetRenderPass
		{
		public:
			RenderPass(FrameGraphBuilder& builder, RscHandle<FrameBuffer> target)
			{
				uvec2 rt_size = target->Size();
				auto& depth_att = target->DepthAttachment();
				auto& depth_tex = depth_att.buffer.as<VknTexture>();
				;
				auto depth = builder.CreateTexture(TextureDescription::make
				(
					name + " depth",
					vk::ImageUsageFlagBits::eDepthStencilAttachment,
					RscHandle<VknTexture>{depth_att.buffer}
				));
				depth = builder.write(depth);

				builder.set_depth_stencil_attachment(depth,
					AttachmentDescription::make(
						vk::AttachmentLoadOp::eClear,
						vk::AttachmentStoreOp::eStore,
						vk::AttachmentLoadOp::eDontCare,
						vk::AttachmentStoreOp::eDontCare,
						depth_tex.FullRange(),
						vk::ClearValue{ vk::ClearDepthStencilValue{1.0f,0} },
						{},
						{},
						vk::ImageLayout::eGeneral
					)
				);
			}
			void Execute(Context_t& context, BaseDrawSet& draw_set) override
			{
				context.DebugLabel(RenderTask::LabelLevel::eWhole, name);
				draw_set.Render(context);
			}
		private:
		};

	}



	

	
	static const mat4 clip_mat = mat4{ vec4{1,0,0,0},vec4{0,1,0,0},vec4{0,0,0.5f,0},vec4{0,0,0.5f,1} };

	void AddDirectionalShadowPass(FrameGraph& frame_graph, GraphicsSystem::LightRenderRange shadow_range, const PreRenderData& state)
	{
		auto& light = state.shared_gfx_state->Lights()[shadow_range.light_index];
		auto& elem = light.light_maps[shadow_range.light_map_index];
		auto cam = CameraData{ Handle<GameObject>{}, light.shadow_layers, light.v, clip_mat * elem.cascade_projection };
		index_span inst_span{ shadow_range.inst_mesh_render_begin, shadow_range.inst_mesh_render_end };
		DirectionalShadow::Bindings bindings;
		bindings::ShadowFilter sf;
		bindings.for_each_binder<>(
		bindings::BinderForward<bindings::SetStateTest>{},
			cam, * state.skeleton_transforms);
		bindings.Get<bindings::VertexShaderBinding>().vertex_shader = Core::GetSystem<GraphicsSystem>().renderer_vertex_shaders[VNormalMesh];
		auto skinned_bindings = bindings;
		skinned_bindings.Get<bindings::VertexShaderBinding>().vertex_shader = Core::GetSystem<GraphicsSystem>().renderer_vertex_shaders[VSkinnedMesh];
		DirectionalShadow::DrawSetBinding derp{
			{
				DirectionalShadow::InstDrawSetBinding{bindings,InstMeshDrawSet{inst_span.to_span(*state.shared_gfx_state->instanced_ros),state.shared_gfx_state->inst_mesh_render_buffer.buffer()}},
				DirectionalShadow::SkinnedInstDrawSetBinding{skinned_bindings,SkinnedMeshDrawSet{state.skinned_mesh_render}}
			}
		};
		frame_graph.addRenderPass<PassSetPair<DirectionalShadow::RenderPass, DirectionalShadow::DrawSetBinding>>("directional light", std::move(derp), elem.light_map);
	}
//#pragma optimize("", off)
	void AddSpotlightShadowPass  (FrameGraph& frame_graph, GraphicsSystem::LightRenderRange shadow_range, const PreRenderData& state)
	{
		auto& light = state.shared_gfx_state->Lights()[shadow_range.light_index];
		auto& elem = light.light_maps[shadow_range.light_map_index];
		auto cam = CameraData{ Handle<GameObject> {}, light.shadow_layers, light.v, light.p };
		index_span inst_span{ shadow_range.inst_mesh_render_begin, shadow_range.inst_mesh_render_end };
		DirectionalShadow::Bindings bindings;
		bindings::ShadowFilter sf;
		bindings.for_each_binder<>(
			bindings::BinderForward<bindings::SetStateTest>{},
			cam, *state.skeleton_transforms);
		bindings.Get<bindings::VertexShaderBinding>().vertex_shader = Core::GetSystem<GraphicsSystem>().renderer_vertex_shaders[VNormalMesh];
		auto skinned_bindings = bindings;
		skinned_bindings.Get<bindings::VertexShaderBinding>().vertex_shader = Core::GetSystem<GraphicsSystem>().renderer_vertex_shaders[VSkinnedMesh];
		DirectionalShadow::DrawSetBinding derp{
			{
				DirectionalShadow::InstDrawSetBinding{bindings,InstMeshDrawSet{inst_span.to_span(*state.shared_gfx_state->instanced_ros),state.shared_gfx_state->inst_mesh_render_buffer.buffer()}},
				DirectionalShadow::SkinnedInstDrawSetBinding{skinned_bindings,SkinnedMeshDrawSet{state.skinned_mesh_render}}
			}
		};
		frame_graph.addRenderPass<PassSetPair<DirectionalShadow::RenderPass, DirectionalShadow::DrawSetBinding>>("spot light", std::move(derp), elem.light_map);
	}
	void AddPointShadowPass      (FrameGraph& frame_graph, GraphicsSystem::LightRenderRange shadow_range, const PreRenderData& state)
	{
		
		auto& light = state.shared_gfx_state->Lights()[shadow_range.light_index];
		auto& elem = light.light_maps[shadow_range.light_map_index];
		
		static array<vec3, 6> cubeMat = {
			vec3{1,0,0},
			vec3{-1,0,0},
			vec3{0,1,0},
			vec3{0,-1,0},
			vec3{0,0,1},
			vec3{0,0,-1}
		};

		vector<mat4> shadow_vp{};

		shadow_vp.emplace_back(light.p * look_at(light.v_pos, light.v_pos + cubeMat[0], cubeMat[3]));
		shadow_vp.emplace_back(light.p * look_at(light.v_pos, light.v_pos + cubeMat[1], cubeMat[3]));
		shadow_vp.emplace_back(light.p * look_at(light.v_pos, light.v_pos + cubeMat[2], cubeMat[4]));
		shadow_vp.emplace_back(light.p * look_at(light.v_pos, light.v_pos + cubeMat[3], cubeMat[5]));
		shadow_vp.emplace_back(light.p * look_at(light.v_pos, light.v_pos + cubeMat[4], cubeMat[3]));
		shadow_vp.emplace_back(light.p * look_at(light.v_pos, light.v_pos + cubeMat[5], cubeMat[3]));

		auto cam = PointCameraData{ Handle<GameObject> {}, light.shadow_layers, light.v, shadow_vp,light.falloff,light.v_pos };
		index_span inst_span{ shadow_range.inst_mesh_render_begin, shadow_range.inst_mesh_render_end };
		PointShadow::Bindings bindings;
		bindings::PointShadowFilter sf;
		bindings.for_each_binder<>(
			bindings::BinderForward<bindings::SetStateTest>{},
			cam, *state.skeleton_transforms);
		bindings.Get<bindings::VertexShaderBinding>().vertex_shader = Core::GetSystem<GraphicsSystem>().renderer_vertex_shaders[VNormalMeshShadow];
		bindings.Get<bindings::GeometryShaderBinding>().geometry_shader = Core::GetSystem<GraphicsSystem>().renderer_geometry_shaders[GPointShadow];
		bindings.Get<bindings::FragmentShaderBinding>().fragment_shader = Core::GetSystem<GraphicsSystem>().renderer_fragment_shaders[FPointShadow];
		auto skinned_bindings = bindings;
		skinned_bindings.Get<bindings::VertexShaderBinding>().vertex_shader = Core::GetSystem<GraphicsSystem>().renderer_vertex_shaders[VSkinnedMeshShadow];
		skinned_bindings.Get<bindings::GeometryShaderBinding>().geometry_shader = Core::GetSystem<GraphicsSystem>().renderer_geometry_shaders[GPointShadow];
		skinned_bindings.Get<bindings::FragmentShaderBinding>().fragment_shader = Core::GetSystem<GraphicsSystem>().renderer_fragment_shaders[FPointShadow];
		PointShadow::DrawSetBinding derp{
			{
				PointShadow::InstDrawSetBinding{bindings,InstMeshDrawSet{inst_span.to_span(*state.shared_gfx_state->instanced_ros),state.shared_gfx_state->inst_mesh_render_buffer.buffer()}},
				PointShadow::SkinnedInstDrawSetBinding{skinned_bindings,SkinnedMeshDrawSet{state.skinned_mesh_render}}
			}
		};
		frame_graph.addRenderPass<PassSetPair<PointShadow::RenderPass, PointShadow::DrawSetBinding>>("point light", std::move(derp), elem.light_map);
		
	}

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