#include "pch.h"
#include "DeferredPass.h"
#include <gfx/FramebufferFactory.h>

#include <vkn/PipelineBinders.h>

#include <vkn/VknRenderTarget.h>
#include <vkn/GraphicsState.h>

#include <vkn/utils/utils.h>

#include <gfx/MeshRenderer.h>
#include <vkn/PipelineBinders.h>
#include <vkn/PipelineBinders.inl>
#include <vkn/FrameRenderer.h>
#if 1
namespace idk::vkn
{
	using EGBufferBinding = meta::enum_info<GBufferBinding, meta::enum_pack<GBufferBinding,
		GBufferBinding::eAlbedoAmbOcc,
		GBufferBinding::eUvMetallicRoughness,
		GBufferBinding::eViewPos,
		GBufferBinding::eNormal,
		GBufferBinding::eTangent>>;

	pipeline_config ConfigWithVP(pipeline_config config, const CameraData& camera, const ivec2& offset, const ivec2& size);
	template<typename T, typename...Args>
	using has_setstate = decltype(std::declval<T>().SetState(std::declval<Args>()...));
	PipelineThingy ProcessRoUniforms(const GraphicsState& state, UboManager& ubo_manager, StandardBindings& binders);
	std::pair<ivec2, ivec2> ComputeVulkanViewport(const vec2& sz, const Viewport& vp);

	void GBufferBarrier(vk::CommandBuffer cmd_buffer, DeferredGBuffer& gbuffer)
	{
		constexpr auto num_buffers = 0;// EGBufferBinding::size();
		std::array<vk::ImageMemoryBarrier, num_buffers + 1> barriers;

		//for (size_t i = 0; i < EGBufferBinding::size(); ++i)
		//{
		//	auto& tex = gbuffer.gbuffer->GetAttachment(i).buffer.as<VknTexture>();
		//	barriers[i] = vk::ImageMemoryBarrier
		//	{
		//		vk::AccessFlagBits::eColorAttachmentWrite,
		//		vk::AccessFlagBits::eShaderRead,
		//		vk::ImageLayout::eColorAttachmentOptimal,
		//		vk::ImageLayout::eShaderReadOnlyOptimal,
		//		*View().QueueFamily().graphics_family,
		//		*View().QueueFamily().graphics_family,
		//		tex.Image(),
		//		vk::ImageSubresourceRange
		//		{
		//			tex.ImageAspects(),
		//			0,1,
		//			0,1,
		//		}
		//	};
		//}
		auto& depth_tex = gbuffer.gbuffer->DepthAttachment().buffer.as<VknTexture>();
		//depth
		barriers[num_buffers] = vk::ImageMemoryBarrier
		{
			{},
			vk::AccessFlagBits::eTransferRead,
			vk::ImageLayout::eGeneral,
			vk::ImageLayout::eTransferSrcOptimal,
			*View().QueueFamily().graphics_family,
			*View().QueueFamily().graphics_family,
			depth_tex.Image(),
			vk::ImageSubresourceRange
			{
				depth_tex.ImageAspects(),
				0,1,
				0,1,
			}
		};
		cmd_buffer.pipelineBarrier(vk::PipelineStageFlagBits::eColorAttachmentOutput, vk::PipelineStageFlagBits::eTransfer, vk::DependencyFlagBits::eByRegion, nullptr, nullptr, barriers, vk::DispatchLoaderDefault{});
	}


	vk::Semaphore DeferredGBuffer::RenderCompleteSignal()
	{
		return *_render_complete;
	}
	void DeferredGBuffer::Init(ivec2 size)
	{
		if (!gbuffer || gbuffer->Size()!=size)
		{
			FrameBufferBuilder fbf;
			fbf.Begin(size);
			//GBufferBinding::eAlbedoAmbOcc
			fbf.AddAttachment(idk::AttachmentInfo{
				LoadOp::eClear,StoreOp::eStore,
				ColorFormat::_enum::RGBAF_32,
				FilterMode::_enum::Nearest
				});
			GBufferBinding::eNormal;
			fbf.AddAttachment(idk::AttachmentInfo{
				LoadOp::eClear,StoreOp::eStore,
				ColorFormat::_enum::RGBAF_32,
				FilterMode::_enum::Nearest
				});
			GBufferBinding::eTangent;
			fbf.AddAttachment(idk::AttachmentInfo{
				LoadOp::eClear,StoreOp::eStore,
				ColorFormat::_enum::RGBAF_32,
				FilterMode::_enum::Nearest
				});
			GBufferBinding::eUvMetallicRoughness;
			fbf.AddAttachment(idk::AttachmentInfo{
				LoadOp::eClear,StoreOp::eStore,
				ColorFormat::_enum::RGBAF_32,
				FilterMode::_enum::Nearest
				});
			GBufferBinding::eViewPos;
			fbf.AddAttachment(idk::AttachmentInfo{
				LoadOp::eClear,StoreOp::eStore,
				ColorFormat::_enum::RGBAF_32,
				FilterMode::_enum::Nearest
				});
			fbf.SetDepthAttachment(
				idk::AttachmentInfo{
					LoadOp::eClear,StoreOp::eStore,
					ColorFormat::_enum::DEPTH_COMPONENT,
					FilterMode::_enum::Nearest
				});
			auto new_buffer = Core::GetResourceManager().GetFactory<FrameBufferFactory>().Create(fbf.End());
			if (gbuffer)
				Core::GetResourceManager().Release(gbuffer);
			gbuffer = RscHandle<VknFrameBuffer>{ new_buffer };
		
			_render_complete = View().Device()->createSemaphoreUnique(vk::SemaphoreCreateInfo{});
		}
	}

	void DeferredPass::Init(ivec2 size) 
	{
		const static renderer_reqs fsq_requirements = 
		{
			{
				{vtx::Attrib::Position,0},
				{vtx::Attrib::UV,1},
			}
		};
		_gbuffer.Init(size); 
		fsq_ro.mesh = Mesh::defaults[MeshType::FSQ];
		fsq_ro.renderer_req = &fsq_requirements;
		if (!fsq_ro.config)
			fsq_ro.config = Core::GetSystem<GraphicsSystem>().MeshRenderConfig();
	}
#if 0

	//Make sure to call this again if the framebuffer size changed.

	void DeferredPass::BindGBuffers(const GraphicsState& graphics_state, RenderStateV2& rs)
	{
		auto& gbuffer = GBuffer();
		auto& view = View();
		//auto& swapchain = view.Swapchain();
		auto dispatcher = vk::DispatchLoaderDefault{};
		vk::CommandBuffer& cmd_buffer = rs.cmd_buffer;
		vk::CommandBufferBeginInfo begin_info{ vk::CommandBufferUsageFlagBits::eOneTimeSubmit,nullptr };
		cmd_buffer.begin(begin_info, dispatcher);

		DrawToGBuffers(cmd_buffer, graphics_state, rs);
		
		auto& camera = graphics_state.camera;



		
		//////////////////Skybox rendering
		if (sb_cm)
		{
			auto& vknCubeMap = sb_cm->as<VknCubemap>();
			pipeline_config skybox_render_config;
			DescriptorsManager skybox_ds_manager(view);
			skybox_render_config.fill_type = FillType::eFill;
			skybox_render_config.prim_top = PrimitiveTopology::eTriangleList;
			auto config = ConfigWithVP(skybox_render_config, camera, offset, size);
			config.vert_shader = Core::GetSystem<GraphicsSystem>().renderer_vertex_shaders[VSkyBox];
			config.frag_shader = Core::GetSystem<GraphicsSystem>().renderer_fragment_shaders[FSkyBox];
			config.cull_face = s_cast<uint32_t>(CullFace::eNone);
			config.depth_test = false;
			config.render_pass_type = BasicRenderPasses::eRgbaColorDepth;

			//No idea if this is expensive....if really so I will try shift up to init
			rs.skyboxRenderer.Init(
				Core::GetSystem<GraphicsSystem>().renderer_vertex_shaders[VSkyBox],
				{},
				Core::GetSystem<GraphicsSystem>().renderer_fragment_shaders[FSkyBox],
				&config,
				*camera.CubeMapMesh
			);
			rs.skyboxRenderer.QueueSkyBox(rs.ubo_manager, {}, *sb_cm, camera.projection_matrix * camera.view_matrix);

			/*cmd_buffer.begin(vk::CommandBufferBeginInfo
				{
					vk::CommandBufferUsageFlagBits::eOneTimeSubmit
				});*/
			rs.skyboxRenderer.ProcessQueueWithoutRP(cmd_buffer, offset, size);
		}

		RenderGbufferToTarget(cmd_buffer,graphics_state,rs);

		if (camera.overlay_debug_draw)
			RenderDebugStuff(state, rs, offset, size);
		rs.ubo_manager.UpdateAllBuffers();
		cmd_buffer.endRenderPass();
		cmd_buffer.end();
	}
#endif
	struct DeferredPostBinder : StandardBindings
	{
		DeferredPass* deferred_pass;
		void SetDeferredPass(DeferredPass& pass)
		{
			deferred_pass = &pass;
		}
		void Bind(PipelineThingy& the_interface) override
		{
			RscHandle<ShaderProgram> fsq_vert = deferred_pass->fullscreen_quad_vert;
			RscHandle<ShaderProgram> deferred_post_frag = deferred_pass->deferred_post_frag;

			the_interface.BindShader(ShaderStage::Vertex, fsq_vert);
			the_interface.BindShader(ShaderStage::Fragment, deferred_post_frag);
			auto& gbuffer_fb = deferred_pass->GBuffer().gbuffer;
			for (uint32_t i = 0; i < EGBufferBinding::size(); ++i)
				the_interface.BindSampler("gbuffers", i, gbuffer_fb->GetAttachment(i).buffer.as<VknTexture>());
		}
		void Bind(PipelineThingy& the_interface, const RenderObject&) override {}
	};

	using PbrDeferredPostBinding = CombinedBindings<DeferredPostBinder, PbrFwdBindings>;

	//
	PipelineThingy DeferredPass::ProcessDrawCalls(const GraphicsState& graphics_state, RenderStateV2& rs)
	{
		//TODO: Prepare FSQ draw call + Forward Draw Calls
		PipelineThingy the_interface{};
		the_interface.SetRef(rs.ubo_manager);
		PbrDeferredPostBinding binding;
		std::get<DeferredPostBinder>(binding.binders).SetDeferredPass(*this);
		std::get<PbrFwdBindings>(binding.binders).SetState(graphics_state);
		
		binding.Bind(the_interface);
		binding.Bind(the_interface, fsq_ro);
		//Draw Fullscreen Quad
		the_interface.FinalizeDrawCall(fsq_ro);
		//Insert Forward stuff here?

		return the_interface;
	}
#if 0
	void CoreRenderPass(const GraphicsState& graphics_state, RenderStateV2& rs)
	{
		DeferredPass* dp;
		rs.cmd_buffer.begin(vk::CommandBufferBeginInfo{ vk::CommandBufferUsageFlagBits::eOneTimeSubmit });
		dp->DrawToGBuffers(rs.cmd_buffer, graphics_state, rs);
		auto the_interface = dp->ProcessDrawCalls(graphics_state, rs);
		the_interface.GenerateDS(rs.dpools);
		//... the rest of the stuff here.

	}
#endif
	bool RenderProcessedDrawCalls(vk::CommandBuffer cmd_buffer,
		const vector<ProcessedRO>& processed_ro,
		const CameraData& camera,
		PipelineManager& pipeline_manager,
		uint32_t frame_index,
		const vk::RenderPassBeginInfo& rpbi,
		VknFrameBuffer& fb
		)
	{
		auto offset = ivec2{ rpbi.renderArea.offset.x,rpbi.renderArea.offset.y };
		auto size= ivec2{ s_cast<uint32_t>(rpbi.renderArea.extent.width),s_cast<uint32_t>(rpbi.renderArea.extent.height )};
		auto& view = View();
		vector<RscHandle<ShaderProgram>> shaders;
		VulkanPipeline* prev_pipeline = nullptr;
		//Draw stuff into the gbuffers
		//auto& processed_ro = the_interface.DrawCalls();
		bool rendered = false;
		for (auto& p_ro : processed_ro)
		{
			rendered = true;
			auto& obj = p_ro.Object();
			if (p_ro.rebind_shaders)
			{
				shaders.resize(0);
				if (p_ro.frag_shader)
					shaders.emplace_back(*p_ro.frag_shader);
				if (p_ro.vertex_shader)
					shaders.emplace_back(*p_ro.vertex_shader);
				if (p_ro.geom_shader)
					shaders.emplace_back(*p_ro.geom_shader);
				
				auto config = ConfigWithVP(*obj.config, camera, offset, size);
				config.attachment_configs.resize(fb.NumColorAttachments());
				auto& pipeline = pipeline_manager.GetPipeline(config, shaders, frame_index, rpbi.renderPass, true);
				pipeline.Bind(cmd_buffer, view);
				SetViewport(cmd_buffer, offset, size);
				prev_pipeline = &pipeline;
			}
			auto& pipeline = *prev_pipeline;
			//TODO Grab everything and render them
			//auto& mat = obj.material_instance.material.as<VulkanMaterial>();
			auto& mesh = obj.mesh.as<VulkanMesh>();
			for (auto& [set, ds] : p_ro.descriptor_sets)
			{
				cmd_buffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, *pipeline.pipelinelayout, set, ds, {});
			}

			auto& renderer_req = *obj.renderer_req;

			for (auto&& [attrib, location] : renderer_req.requirements)
			{
				auto& attrib_buffer = mesh.Get(attrib);
				cmd_buffer.bindVertexBuffers(*pipeline.GetBinding(location), *attrib_buffer.buffer(), vk::DeviceSize{ attrib_buffer.offset }, vk::DispatchLoaderDefault{});
			}

			auto& oidx = mesh.GetIndexBuffer();
			if (oidx)
			{
				cmd_buffer.bindIndexBuffer(*(*oidx).buffer(), 0, mesh.IndexType(), vk::DispatchLoaderDefault{});
				cmd_buffer.drawIndexed(mesh.IndexCount(), 1, 0, 0, 0, vk::DispatchLoaderDefault{});
			}
		}
		return rendered;
	}

	void DeferredPass::DrawToGBuffers(vk::CommandBuffer cmd_buffer,const GraphicsState& graphics_state,RenderStateV2& rs)
	{
		auto& view = View();
		auto& gbuffer = GBuffer();
		//Bind the material uniforms
		PbrFwdMaterialBinding binder;
		binder.for_each_binder<has_setstate>([](auto& binder, const GraphicsState& state) { binder.SetState(state); }, graphics_state);

		//Preprocess MeshRender's uniforms
		auto&& the_interface = ProcessRoUniforms(graphics_state, rs.ubo_manager, binder);
		the_interface.GenerateDS(rs.dpools,false);



		std::array<float, 4> a{};

		//auto& cd = std::get<vec4>(state.camera.clear_data);
		//TODO grab the appropriate framebuffer and begin renderpass

		//auto& vvv = state.camera.render_target.as<VknFrameBuffer>();

		auto& camera = graphics_state.camera;
		//auto default_frame_buffer = *swapchain.frame_buffers[swapchain.curr_index];
		auto& vkn_fb = camera.render_target.as<VknRenderTarget>();
		auto& g_buffer = *gbuffer.gbuffer;
		auto frame_buffer = g_buffer.GetFramebuffer();
		//TransitionFrameBuffer(camera, cmd_buffer, view);

		auto sz = camera.render_target->Size();
		auto [offset, size] = ComputeVulkanViewport(vec2{ sz }, camera.viewport);

		std::array<float, 4> depth_clear{ 1.0f,1.0f ,1.0f ,1.0f };
		std::array<float, 4> g_clear{ 0.0f,0.0f ,0.0f ,0.0f };

		vk::ClearValue v[EGBufferBinding::size()+1]{};
		for(auto& value : v)
		{
			value = vk::ClearValue{ g_clear };
		};
		v[EGBufferBinding::size()] =vk::ClearValue{ vk::ClearColorValue{ depth_clear } };

		vk::Rect2D render_area
		{
			vk::Offset2D
			{
				s_cast<int32_t>(offset.x),s_cast<int32_t>(offset.y)
			},vk::Extent2D
			{
				s_cast<uint32_t>(size.x),s_cast<uint32_t>(size.y)
			}
		};
		vk::RenderPassBeginInfo rpbi
		{
			g_buffer.GetRenderPass(), frame_buffer,
			render_area,hlp::arr_count(v),std::data(v)
		};

		cmd_buffer.beginRenderPass(rpbi, vk::SubpassContents::eInline);
		if (RenderProcessedDrawCalls(cmd_buffer, the_interface.DrawCalls(), camera, pipeline_manager(), frame_index(), rpbi,g_buffer))
			rs.FlagRendered();
		cmd_buffer.endRenderPass();//End GBuffer pass
		GBufferBarrier(cmd_buffer, gbuffer);
	}
}
#endif