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
	namespace PointShadow
	{
		using Bindings = bindings::ShadowBinding;
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
	namespace meta
	{

	namespace detail
	{
		template<typename IndexSeq>
		struct reverse_seq;
		template< size_t ... Nums>
		struct reverse_seq<std::index_sequence<Nums...>>
		{
			static constexpr size_t N = sizeof...(Nums);
			using type = std::index_sequence<(N - Nums)...>;
		};

		template<typename index_seq, typename tuple>
		struct shuffle;
		template<size_t ...indices,typename tuple>
		struct shuffle<std::index_sequence<indices...>,tuple>
		{
			using type = std::tuple<std::tuple_element_t<indices, tuple>...>;
		};

		template<typename T>
		struct Reverse;
		template<typename ...Args, template<typename...> typename tuple>
		struct Reverse<tuple<Args...>>
		{
			using seq_t = std::make_index_sequence<sizeof...(Args)>;
			using rev_seq_t = detail::reverse_seq<seq_t>;
			using type =typename detail::shuffle<rev_seq_t, tuple<Args...>>::type;
		};


		template<size_t max_items, typename T>
		struct Cull;

		template<size_t N, typename ...Args, template<typename...> typename tuple>
		struct Cull<N, tuple<Args...>>
		{
			using seq_t = std::make_index_sequence<std::min(N, sizeof...(Args))>;
			using type = typename detail::shuffle<seq_t, tuple<Args...>>::type;
		};

		template<typename func, typename tuple, typename = void>
		struct ApplierHelper2 : std::false_type {};
		template<typename Func, template<typename...>typename tuple, typename ...Args>
		struct ApplierHelper2<Func, tuple<Args...>, decltype(std::declval<Func>()(std::declval<Args&&>()...))> :std::true_type
		{
			template<typename Func, typename ...OtherArgs>
			static void apply(Func&& func, Args&&... args, OtherArgs&&...)
			{
				func(std::forward<Args>(args)...);
			}
		};
		template<size_t N, typename ...Args>
		struct ApplierHelper 
		{
			template<typename Func>
			static void apply(Func&& func, Args&&... args)
			{
				using culled = typename Cull<N, std::tuple<Args...>>::type;
				using Check = ApplierHelper2<Func, culled>;
				if constexpr (Check::value)
				{
					Check::apply(std::forward<Func>(func),std::forward<Args>(args)...);
				}
				else if constexpr (N > 1)
				{
					ApplierHelper<N - 1, Args...>::apply(std::forward<Func>(func), std::forward<Args>(args)...);
				}

			}
			template<typename Func>
			void operator()(Func&& func, Args&&... args)
			{
				apply(std::forward<Func>(func), std::forward<Args>(args)...);
			}
		};
	}

	template<size_t N, typename tuple>
	using cull_t = detail::Cull<N, tuple>;


	void AAAAAA()
	{

	}
	template<typename ...Args>
	struct Applier : detail::ApplierHelper<sizeof...(Args),Args...>
	{
		using base = detail::ApplierHelper<sizeof...(Args), Args...>;
		using base::operator(); //use all the other valid functors
	};

	}


	template<typename Binder, typename ...Args>
	void SetStateTestFunc(Binder&& binder, Args&&... args)
	{
		volatile int a = 0;
		a += 0;
		binder.SetState(std::forward<Args>(args)...);
	}

	struct SetStateTest
	{
		template<typename Binder, typename ...Args, typename = decltype(std::declval<Binder>().SetState(std::declval<Args&&>()...)) >
		void operator()(Binder&& binder, Args&&... args)
		{
			SetStateTestFunc(std::forward<Binder>(binder), std::forward<Args>(args)...);
		}
		
	};


	template<typename Binder, typename ...Args>
	void SetStateTest2Func(Binder&& binder, Args&&... args)
	{
		SetStateTest hmm{};
		meta::Applier<Binder, Args...> applier;
		volatile int a = 0;
		a += 0;
		applier(hmm, std::forward<Binder>(binder), std::forward<Args>(args)...);
	}
	struct SetStateTest2
	{
		template<typename Binder, typename ...Args>
		void operator()(Binder&& binder, Args&&... args)
		{
			SetStateTest2Func(std::forward<Binder>(binder), std::forward<Args>(args)...);
		}

	};

	



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
		bindings::ShadowFilter sf;
		bindings.for_each_binder<>(
			SetStateTest2{},
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
			SetStateTest2{},
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
		/*auto& light = state.shared_gfx_state->Lights()[shadow_range.light_index];
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

		auto cam = CameraData{ Handle<GameObject> {}, light.shadow_layers, light.v, light.p };
		index_span inst_span{ shadow_range.inst_mesh_render_begin, shadow_range.inst_mesh_render_end };
		PointShadow::Bindings bindings;
		bindings::ShadowFilter sf;
		bindings.for_each_binder<>(
			SetStateTest2{},
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
		frame_graph.addRenderPass<PassSetPair<DirectionalShadow::RenderPass, DirectionalShadow::DrawSetBinding>>("point light", std::move(derp), elem.light_map);*/

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