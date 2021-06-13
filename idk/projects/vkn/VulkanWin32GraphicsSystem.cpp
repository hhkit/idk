#include <pch.h>
#include <iostream>
#include <vkn/VulkanWin32GraphicsSystem.h>
#include <WindowsApplication.h>

#include <vkn/VulkanState.h>
#include <vkn/VulkanMesh.h>
#include <vkn/VulkanDebugRenderer.h>
#include <core/Core.h>
#include <editor/IEditor.h>

#include <vkn/VulkanPipeline.h>
#include <gfx/pipeline_config.h>
#include <file/FileSystem.h>
#include <vkn/PipelineManager.h>
#include <vkn/RegisterVulkanFactories.h>

#include <glslang/public/ShaderLang.h>
#include <vkn/TmpTest.h>

#include <vkn/VknRenderTarget.h>

#include <vkn/VknCubeMapLoader.h>
#include <vkn/VknTextureLoader.h>
#include <vkn/VknFrameBufferFactory.h>
#include <res/ResourceManager.inl>
#include <res/ResourceHandle.inl>
#include <res/ResourceMeta.inl>
#include <core/SystemManager.inl>
#include <ds/result.inl>

#include <vkn/DebugUtil.h>

#include <vkn/MaterialInstanceCache.h>

#include <time.h>

#include<vkn/UboManager.h>
#include <vkn/FrameRenderer.h>

#include <vkn/time_log.h>
#include <vkn/Stopwatch.h>
#include <vkn/SubDurations.h>

#include <vkn/VknAsyncTexLoader.h>
#include <vkn/AsyncLoaders.h>
#include <vkn/VulkanMeshFactory.h>
#include <vkn/ExtraConfigs.h>
bool operator<(const idk::Guid& lhs, const idk::Guid& rhs)
{
	using num_array_t = const uint64_t[2];
	static_assert(sizeof(idk::Guid)== sizeof(num_array_t), "Guid size is not as expected, please update comparison op.");
	auto& l = idk::r_cast<num_array_t&>(lhs);
	auto& r = idk::r_cast<num_array_t&>(rhs);
	return l[0] < r[0] || (l[0] == r[0] && l[1] < r[0]);
}

bool operator<(const idk::RscHandle<idk::RenderTarget>& lhs, const idk::RscHandle<idk::RenderTarget>& rhs)
{
	return lhs.guid < rhs.guid;
}

//static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
//	[[maybe_unused]] VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
//	[[maybe_unused]] VkDebugUtilsMessageTypeFlagsEXT messageType,
//	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
//	[[maybe_unused]] void* pUserData) {
//
//	std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
//
//	return VK_FALSE;
//}
//
//VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT& pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
//	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
//	if (func != nullptr) {
//		return func(instance, &pCreateInfo, pAllocator, pDebugMessenger);
//	}
//	else {
//		return VK_ERROR_EXTENSION_NOT_PRESENT;
//	}
//}


namespace idk::vkn
{
	struct VulkanWin32GraphicsSystem::Pimpl
	{
		std::unique_ptr<hlp::MemoryAllocator> allocator;
		vk::UniqueFence fence;
		bool rendered_brdf = false;
		dbg::time_log timelog;
		dbg::stopwatch timer;
		RscHandle<Texture> BrdfLookupTable;
		AsyncTexLoader tex_loader;
		size_t initial_texture_count=0;
		bool inited = false;
		AsyncLoaders async_loaders;
	};
	dbg::time_log& GetGfxTimeLog()
	{
		return Core::GetSystem<VulkanWin32GraphicsSystem>().TimeLog();
	}

	VulkanWin32GraphicsSystem::VulkanWin32GraphicsSystem() :  instance_{ std::make_unique<VulkanState>() }
	{
		if (!instance_)
			throw std::runtime_error("Failed to instantiate VulkanState");
		assert(instance_);
		instance_->extra_configs = &GetExtraConfigs();
	}
	VulkanWin32GraphicsSystem::~VulkanWin32GraphicsSystem()
	{
	}
	void VulkanWin32GraphicsSystem::SetExtraConfigs(const ExtraConfigs& extra_configs)
	{
		if (!_extra_configs)
			_extra_configs = std::make_unique<ExtraConfigs>();
		*_extra_configs = extra_configs;
	}
	ExtraConfigs& VulkanWin32GraphicsSystem::GetExtraConfigs() 
	{
		if (!_extra_configs)
			_extra_configs = std::make_unique<ExtraConfigs>();
		return *_extra_configs;
	}
	void VulkanWin32GraphicsSystem::Init()
	{
		try
		{

			windows_ = &Core::GetSystem<win::Windows>();
			instance_->InitVulkanEnvironment(window_info{ windows_->GetScreenSize(),windows_->GetWindowHandle(),windows_->GetInstance() });
			windows_->OnScreenSizeChanged.Listen([this](const ivec2&) { Instance().OnResize(); });

			//if (!instance_)
			//	throw;
			RegisterFactories();
			_pm = std::make_unique<PipelineManager>();

			_pimpl = std::make_unique<Pimpl>();
			_pimpl->allocator = std::make_unique<hlp::MemoryAllocator>(*instance_->View().Device(), instance_->View().PDevice());
			_pimpl->fence = instance_->View().Device()->createFenceUnique({});

			_pimpl->async_loaders.AddLoader(std::dynamic_pointer_cast<IAsyncLoader>(Core::GetResourceManager().GetFactory<MeshFactory>().async_loader));
		}
		catch (std::exception& e)
		{
			LOG_CRASH_TO(LogPool::GFX, "Failed to init vulkan system %s", e.what());
			throw;//Rethrow
		}
		catch (vk::Error& e)
		{
			LOG_CRASH_TO(LogPool::GFX, "Failed to init vulkan system %s", e.what());
			throw;//Rethrow
		}
		catch (...)
		{
			LOG_CRASH_TO(LogPool::GFX, "Failed to init vulkan system, unknown exception type thrown");
			throw;//Rethrow
		}
	}
	void VulkanWin32GraphicsSystem::RenderBRDF(RscHandle<ShaderProgram> prog)
	{
		auto& view = instance_->View();
		;
		vk::CommandBufferAllocateInfo ai
		{
			*view.Commandpool(),vk::CommandBufferLevel::ePrimary,1
		};
		auto ucmd_buffer = view.Device()->allocateCommandBuffersUnique(ai);
		vk::CommandBuffer cmd_buffer = *ucmd_buffer.front();
		auto brdf_texture = _pimpl->BrdfLookupTable;
		pipeline_config brdf_config ;

		auto shaders = {
			*Core::GetResourceManager().Load<ShaderProgram>("/engine_data/shaders/fsq.vert", false),
			RscHandle<ShaderProgram>{prog}
		};

		auto& brdf_pipeline = _pm->GetPipeline(brdf_config, shaders, 0);
		FrameBufferBuilder builder;
		builder.Begin("BRDF",brdf_texture->Size());
		builder.AddAttachment(
			AttachmentInfo
			{
				LoadOp::eDontCare,
				StoreOp::eStore,
				{},
				{},
				false,
				brdf_texture
			}
		);
		auto fb = Core::GetResourceManager().GetFactory<FrameBufferFactory>().Create(builder.End());
				
		cmd_buffer.begin(vk::CommandBufferBeginInfo{ vk::CommandBufferUsageFlagBits::eOneTimeSubmit });
		auto& vfb = fb.as<VknFrameBuffer>();
		auto clear_val = vk::ClearValue{ vk::ClearColorValue{} };
		vk::RenderPassBeginInfo rpbi
		{
			*vfb.GetRenderPass(),vfb.GetFramebuffer(),
			vk::Rect2D
			{
				vk::Offset2D{},vk::Extent2D{s_cast<uint32_t>(vfb.Size().x),s_cast<uint32_t>(vfb.Size().y)}
			},1,&clear_val
		};
		brdf_texture->Name("Brdf");
		cmd_buffer.beginRenderPass(rpbi,vk::SubpassContents::eInline);
		
		VulkanMesh& mesh =  Mesh::defaults[MeshType::FSQ].as<VulkanMesh>();
		auto req = renderer_attributes{};
		req.mesh_requirements = {
			std::make_pair(vtx::Attrib::Position, 0),
			std::make_pair(vtx::Attrib::UV, 1) };

		brdf_pipeline.Bind(cmd_buffer, View());
		cmd_buffer.setViewport(0, vk::Viewport{ 0,0,s_cast<float>(brdf_texture->Size().x),s_cast<float>(brdf_texture->Size().y),0.0f,1.0f });

		for (auto&& [attrib, location] : req.mesh_requirements)
		{
			auto& attrib_buffer = mesh.Get(attrib);
			cmd_buffer.bindVertexBuffers(*brdf_pipeline.GetBinding(location), *attrib_buffer.buffer(), vk::DeviceSize{ attrib_buffer.offset }, vk::DispatchLoaderDefault{});
		}

		auto& oidx = mesh.GetIndexBuffer();
		if (oidx)
		{
			cmd_buffer.bindIndexBuffer(*(*oidx).buffer(), 0, mesh.IndexType(), vk::DispatchLoaderDefault{});
			cmd_buffer.drawIndexed(mesh.IndexCount(), 1, 0, 0, 0, vk::DispatchLoaderDefault{});
		}
		cmd_buffer.endRenderPass();
		cmd_buffer.end();
		auto device = *View().Device();
		vk::UniqueFence fence = device.createFenceUnique(vk::FenceCreateInfo{});
		
		instance_->View().GraphicsQueue().submit(vk::SubmitInfo
			{
				0,nullptr,nullptr,1,&cmd_buffer
			}, *fence);
		while (device.getFenceStatus(*fence) == vk::Result::eNotReady);
		_pm->RemovePipeline(&brdf_pipeline);
		Core::GetResourceManager().Release(fb);
		_pimpl->rendered_brdf = true;
	}
	void VulkanWin32GraphicsSystem::LateInit()
	{
		try
		{

		GraphicsSystem::LateInit();
		auto queue_families = View().PDevice().getQueueFamilyProperties();
		
		LOG_TO(LogPool::GFX, "Graphics Queue Family has %d queues", (int)queue_families[*View().QueueFamily().graphics_family].queueCount);
		_debug_renderer = std::make_unique<VulkanDebugRenderer>();
		_debug_renderer->Init();
		_frame_renderers.resize(instance_->MaxFramesInFlight());//instance_->View().Swapchain().frame_objects.size());
		for (auto& frame : _frame_renderers)
		{
			frame.Init(&instance_->View(), *instance_->View().Commandpool());
			frame.SetPipelineManager(*_pm);
		}
		instance_->imguiEnabled = s_cast<bool>(&Core::GetSystem<IEditor>());

		auto& brdf_texture =  _pimpl->BrdfLookupTable = Core::GetResourceManager().Create<Texture>();

		TextureLoader loader;
		TextureOptions options; 
		options.internal_format = TextureInternalFormat::RG_16_F;

		TexCreateInfo info = ColorBufferTexInfo(512, 512);
		info.image_usage |= vk::ImageUsageFlagBits::eColorAttachment;
		loader.LoadTexture(brdf_texture.as<VknTexture>(), *_pimpl->allocator, *_pimpl->fence, options, info, {});
		}
		catch (std::exception& e)
		{
			LOG_CRASH_TO(LogPool::GFX, "Failed to late init vulkan system %s", e.what());
			throw;//Rethrow
		}
		catch (vk::Error& e)
		{
			LOG_CRASH_TO(LogPool::GFX, "Failed to late init vulkan system %s", e.what());
			throw;//Rethrow
		}
		catch (...)
		{
			LOG_CRASH_TO(LogPool::GFX, "Failed to late init vulkan system, unknown exception type thrown");
			throw;//Rethrow
		}

	}

	namespace hlp
	{
		string DumpAllocators();
		std::pair<size_t, size_t> DumpAllocator(std::ostream& out, const MemoryAllocator& alloc);
	}
	void ReloadDeferredShaders()
	{
		Core::GetSystem<GraphicsSystem>().LoadShaders();
	}

	void profile_bp_start();
	void profile_bp_end();
	void VulkanWin32GraphicsSystem::RenderRenderBuffer()
	{
		if (instance_->m_ShouldResize)
			instance_->RecreateSwapChain();
		if (!_pimpl->inited)
		{
			_pimpl->initial_texture_count = _pimpl->tex_loader.num_pending();
			_pimpl->inited = true;
		}
		_pimpl->timelog.reset();
		_pimpl->timer.start();
		_pimpl->timelog.start("Total");
		_pimpl->timelog.start("Pre Acquire Frame");
		profile_bp_start();
		auto d_start = std::chrono::high_resolution_clock::now();

		auto dump = []() {

			auto fb_dump = dbg::DumpFrameBufferAllocs();
			auto rt_dump = dbg::DumpRenderTargetAllocs();
			auto alloc_dump = dbg::DumpMemoryAllocs();

			LOG_CRASH_TO(LogPool::GFX, "Framebuffer Dump %s", fb_dump.c_str());
			LOG_CRASH_TO(LogPool::GFX, "RenderTarget Dump %s", rt_dump.c_str());
			LOG_CRASH_TO(LogPool::GFX, "MemoryAllocator Dump %s", alloc_dump.c_str());
		};

		try
		{

		
		_pimpl->timelog.end_then_start("Acquire Frame");
			auto& curr_signal = instance_->View().CurrPresentationSignals();

		auto d_af_start = std::chrono::high_resolution_clock::now();

		if (instance_->extent.height * instance_->extent.width == 0)
			return; //Skip rendering if there's nothing to render

		instance_->AcquireFrame(*curr_signal.image_available);
		_pimpl->timelog.end_then_start("Process Updated Resources");
		auto d_af_end = std::chrono::high_resolution_clock::now();
		//auto e_time = (d_af_end - d_af_start).count();
		//LOG_CRASH_TO(LogPool::GFX, "Acquired Frame time: %d", std::chrono::duration_cast<std::chrono::microseconds>((d_af_end - d_af_start)).count());

		auto curr_index = instance_->View().CurrFrame();
		instance_->ResourceManager().ProcessQueue(curr_index);
		{
			auto& var = extra_vars;
			auto name = "Reload Deferred";
			var.SetIfUnset(name, false);
			if (*var.Get<bool>(name))
			{
				ReloadDeferredShaders();
				var.Set(name, false);
			}
		}
		_pimpl->timelog.end_then_start("Update Pipelines");
		auto& curr_frame = _frame_renderers[curr_index];
		auto& curr_buffer = object_buffer[curr_draw_buffer];
		_pm->CheckForUpdates(curr_index);
		{
			auto& var = extra_vars;
			auto name = "Reload Textures";
			var.SetIfUnset(name, false);
			if (*var.Get<bool>(name))
			{
				_pimpl->tex_loader.ClearQueue();
				auto textures = Core::GetResourceManager().GetAll<VknTexture>();
				for (auto& tex : textures)
				{
					tex->BeginAsyncReload();
				}
				
				_pimpl->initial_texture_count = _pimpl->tex_loader.num_pending();
				var.Set(name, false);
			}
		}
		{
			auto& tl_state = _pimpl->tex_loader.state;
			extra_vars.Set("tl:queued", (int)tl_state.pending_queue);
			extra_vars.Set("tl:q 4 q 4 thds", (int)tl_state.pending_scratch_queue);
			extra_vars.Set("tl:before_update_future_present", tl_state.has_future_before_update);
			extra_vars.Set("tl:after_update_future_present", tl_state.has_future_after_update);
			extra_vars.Set("tl:wait_load_complete", tl_state.wait_results);
			extra_vars.Set("tl:wait_load2_complete", tl_state.wait_results2);
			extra_vars.Set("tl:1_loading", tl_state.load1);
			extra_vars.Set("tl:2_loading", tl_state.load2);
		}

		extra_vars.Set("pending_textures", (int)_pimpl->tex_loader.num_pending());
		extra_vars.Set("pending_resources", (int)(_pimpl->async_loaders.NumAdded() - _pimpl->async_loaders.NumProcessed() + _pimpl->tex_loader.num_pending()));
		extra_vars.Set("total_async_resources", (int)(_pimpl->async_loaders.NumAdded() + _pimpl->initial_texture_count ) );
		extra_vars.Set("async_resources_loaded", (int)( _pimpl->async_loaders.NumProcessed()+ (_pimpl->initial_texture_count -_pimpl->tex_loader.num_pending())));
		extra_vars.Set("async tex loading exception", _pimpl->tex_loader.state.future_err);
		extra_vars.Set("async loading exceptions", _pimpl->async_loaders.future_errs);
		_pimpl->tex_loader.UpdateTextures();
		_pimpl->async_loaders.UpdateLoaders();
		_pimpl->timelog.end_then_start("Init Pre render data");

		std::vector<GraphicsState> curr_states(curr_buffer.camera.size());

		_debug_renderer->GrabDebugBuffer();

		SharedGraphicsState& shared_graphics_state=curr_frame.shared_graphics_state;
		shared_graphics_state.Reset();
		auto& active_lights = curr_buffer.active_light_buffer;
		auto& lights = curr_buffer.lights;
		shared_graphics_state.mat_inst_cache = &curr_frame.GetMatInstCache();
		shared_graphics_state.Init(lights,curr_buffer.instanced_mesh_render);
		shared_graphics_state.ProcessMaterialInstances(curr_buffer.active_materials);
		shared_graphics_state.BrdfLookupTable = _pimpl->BrdfLookupTable;
		shared_graphics_state.particle_data = &curr_buffer.particle_buffer;
		shared_graphics_state.particle_range = &curr_buffer.particle_range;

		shared_graphics_state.characters_data = &curr_buffer.font_buffer;
		shared_graphics_state.fonts_data = &curr_buffer.font_render_data;
		shared_graphics_state.font_range = &curr_buffer.font_range;

		//shared_graphics_state.ui_render_per_canvas = &curr_buffer.ui_render_per_canvas;
		shared_graphics_state.ui_canvas = &curr_buffer.ui_canvas;
		shared_graphics_state.ui_attrib_data = &curr_buffer.ui_attrib_buffer;
		shared_graphics_state.ui_text_range = &curr_buffer.ui_text_range;
		shared_graphics_state.total_num_of_text = curr_buffer.ui_total_num_of_text;
		//shared_graphics_state.ui_canvas_range = &curr_buffer.ui_canvas_range;

		PreRenderData pre_render_data;
		pre_render_data.shared_gfx_state = &shared_graphics_state;
		pre_render_data.active_lights.reserve(lights.size());
		pre_render_data.active_dir_lights.reserve(curr_buffer.directional_light_buffer.size());
		pre_render_data.cameras = &curr_buffer.camera;
		pre_render_data.d_lightmaps = &curr_buffer.d_lightmaps;

		for (auto i: active_lights)// size_t i = 0; i < lights.size(); ++i)
			if(lights[i].index!=0)
				pre_render_data.active_lights.emplace_back(i);

		for (auto& elem : curr_buffer.directional_light_buffer)
			pre_render_data.active_dir_lights.emplace_back(elem);

		pre_render_data.Init(curr_buffer.mesh_render, curr_buffer.skinned_mesh_render, curr_buffer.skeleton_transforms,curr_buffer.inst_mesh_render_buffer);
		if (&curr_buffer.skeleton_transforms != pre_render_data.skeleton_transforms)
			throw std::runtime_error("Skeleton Transforms corrupted");
		_pimpl->timelog.end_then_start("Init render data");

		pre_render_data.shadow_ranges = &curr_buffer.culled_light_render_range;

		shared_graphics_state.renderer_vertex_shaders = curr_buffer.renderer_vertex_shaders;
		shared_graphics_state.renderer_fragment_shaders = curr_buffer.renderer_fragment_shaders;

		PostRenderData post_render_data;
		post_render_data.shared_gfx_state = &shared_graphics_state;
		post_render_data.cameras = &curr_buffer.camera;
		post_render_data.d_lightmaps = &curr_buffer.d_lightmaps;
		//post_render_data.canvas_render_range = &curr_buffer.canvas_render_range;
		//post_render_data.Init();

		
		hash_set<RscHandle<RenderTarget>> render_targets;

		auto IsDontClear = [](const CameraData& camera)
		{
			return camera.clear_data.index() == meta::IndexOf <std::remove_const_t<decltype(camera.clear_data)>, DontClear>::value;
		};
		
		bool will_draw_debug = true;
		_pimpl->timelog.end_then_start("Init render data");
		for (size_t i = 0; i < curr_states.size()&&i<curr_buffer.culled_render_range.size(); ++i)
		{
		_pimpl->timelog.start("b4 process material");
			dbg::stopwatch inner_timer{};
			inner_timer.start();
			auto& curr_state = curr_states[i];		
			auto& curr_range = curr_buffer.culled_render_range[i];
			auto& curr_cam = curr_range.camera;

			auto& pimpl = _pimpl;
			std::visit([&](auto& clear)
			{
				if constexpr (std::is_same_v<std::decay_t<decltype(clear)>, RscHandle<CubeMap>>)
				{
					const RscHandle<CubeMap>& cubemap = clear;
					if (!cubemap)
						return;
					VknCubemap& cm = cubemap.as<VknCubemap>();
					RscHandle<VknCubemap> conv = cm.GetConvoluted();
					if (RscHandle < VknCubemap>{} == conv)
					{
						conv = Core::GetResourceManager().Create<VknCubemap>();
						conv->Size(cubemap->Size());
						CubemapLoader loader;
						CubemapOptions options{ cm.GetMeta() };
						CMCreateInfo info = CMColorBufferTexInfo(cubemap->Size().x, cubemap->Size().y);
						info.image_usage |= vk::ImageUsageFlagBits::eColorAttachment;
						loader.LoadCubemap(conv.as<VknCubemap>(), *pimpl->allocator, *pimpl->fence, options, info, {});
						cm.SetConvoluted(conv);
					}
				}
			}, curr_cam.clear_data);

			//curr_state.ppEffect = ppEffect;
			//Init render datas (range for instanced data, followed by render datas for other passes)
			curr_state.Init(curr_range,curr_buffer.active_light_buffer,curr_buffer.directional_light_buffer, curr_buffer.lights,curr_buffer.d_lightmaps, curr_buffer.mesh_render, curr_buffer.skinned_mesh_render,curr_buffer.skeleton_transforms);
			const auto itr = render_targets.find(curr_cam.render_target);
			
			curr_state.clear_render_target = !IsDontClear(curr_cam);

			if(itr==render_targets.end())
				render_targets.emplace(curr_cam.render_target);

			//curr_state.mesh_vtx = curr_buffer.mesh_vtx;
			//curr_state.skinned_mesh_vtx = curr_buffer.skinned_mesh_vtx;
			curr_state.dbg_render.resize(0);
			curr_state.shared_gfx_state = &shared_graphics_state;
			;
		//_pimpl->timelog.end_then_start("process material");
		//	curr_state.ProcessMaterialInstances(shared_graphics_state.material_instances);
		_pimpl->timelog.end_then_start("dbg stuff");
			if (curr_cam.render_target->RenderDebug())
			{
				will_draw_debug = true;
				curr_state.dbg_pipeline = &_debug_renderer->GetPipeline();
				//TODO Add cull step
				curr_state.dbg_render.reserve(std::size(_debug_renderer->DbgDrawCalls()));
				for (auto& [mesh,dbgcall] : _debug_renderer->DbgDrawCalls())
				{
					if(dbgcall.num_instances)
						curr_state.dbg_render.emplace_back(&dbgcall);
				}
			}
			_pimpl->timelog.end();// "dbg stuff", inner_timer.lap());
			//_debug_renderer->Render(curr_state.camera.view_matrix, mat4{1,0,0,0,   0,-1,0,0,   0,0,0.5f,0.5f, 0,0,0,1}*curr_state.camera.projection_matrix);
		}
		_pimpl->timelog.end_then_start("Finalize render targets");

		if (will_draw_debug)
		{
			for (auto& [buffer, data] : _debug_renderer->BufferUpdateInfo())
			{
				shared_graphics_state.update_instructions.emplace_back(BufferUpdateInst{buffer,data,0});
			}
		}
		//render_targets.emplace(RscHandle<VknRenderTarget>{});
		for (auto& prt : render_targets)
		{
			if (prt->NeedsFinalizing())
				prt->Finalize();
		}
		_pimpl->timelog.end_then_start("Process Material Cache");
		{
			auto& mat_cache = curr_frame.GetMatInstCache();
			hash_set<ShaderModule*> shaders;
			for (auto& [handle, p_mat] : shared_graphics_state.material_instances)
			{
				shaders.emplace(&p_mat.shader.as<ShaderModule>());
			}
			for (auto& shader : shaders)
			{
				shader->UpdateCurrent(curr_index);
			}
			mat_cache.Start();
			for (auto& [handle, p_mat] : shared_graphics_state.material_instances)
			{
				mat_cache.CacheMaterialInstance(p_mat);
			}
			mat_cache.ProcessCreation();
		}
		_pimpl->timelog.end_then_start("Color pick init");
		if (RscHandle<VknRenderTarget>{}->NeedsFinalizing())
			RscHandle<VknRenderTarget>{}->Finalize();
		// */
		//string test = hlp::DumpAllocators();



		curr_frame.ColorPick(std::move(request_buffer));
		_pimpl->timelog.end_then_start("Pre render");
		curr_frame.PreRenderGraphicsStates(pre_render_data, curr_index); //TODO move this to Prerender
#if 0
		{
			auto str = dbg::DumpFrameBufferAllocs();
		}
#endif
		//std::reverse(curr_states.begin(), curr_states.end());
		_pimpl->timelog.end_then_start("Render");
		curr_frame.RenderGraphicsStates(curr_states, curr_index);
#if 0
		{
			auto str = dbg::DumpFrameBufferAllocs();
		}
#endif
		_pimpl->timelog.end_then_start("Post Render");
		curr_frame.PostRenderGraphicsStates(post_render_data, curr_index);
		_pimpl->timelog.end_then_start("Misc");


		//vk::PipelineStageFlags stage = vk::PipelineStageFlagBits::eColorAttachmentOutput;
		//vk::SubmitInfo si
		//{
		//	1,
		//	&*curr_frame.GetPostRenderComplete(),
		//	&stage,0,nullptr,1,
		//	&* curr_signal.render_finished
		//};
		//View().GraphicsQueue().submit(si, {});
		vector<RscHandle<RenderTarget>> targets{render_targets.begin(),render_targets.end()};
		instance_->DrawFrame(*curr_frame.GetPostRenderComplete(), *curr_signal.render_finished, targets);
		auto d_end = std::chrono::high_resolution_clock::now();

		//LOG_CRASH_TO(LogPool::GFX, "renderrender time: %d", std::chrono::duration_cast<std::chrono::microseconds>((d_end - d_start)).count());
		//LOG_CRASH_TO(LogPool::GFX, "Render End Log: %s", );
		if (extra_vars.GetOptional<bool>("Dump", false))
		{
			extra_vars.Set("Dump", false);
			dump();
		}

		}
		catch (vk::OutOfDateKHRError& err)
		{
			LOG_CRASH_TO(LogPool::GFX, "Vulkan Error in renderenderbuffer: %s", err.what());
			//non-fatal
		}
		catch (vk::SystemError& err)
		{
			LOG_CRASH_TO(LogPool::GFX, "Vulkan Error in renderenderbuffer: %s", err.what());
			dump();
			throw; //Rethrow
		}
		catch (std::exception& e)
		{
			LOG_CRASH_TO(LogPool::GFX, "Exception in renderenderbuffer: %s", e.what());
			dump();
			throw;//Rethrow
		}
		catch (...)
		{
			LOG_CRASH_TO(LogPool::GFX, "Unknown exception thrown in renderenderbuffer");
			throw;//Rethrow
		}
		profile_bp_end();
		_pimpl->timelog.end();
		_pimpl->timer.stop();
		_pimpl->timelog.end();// "Total");
		//_pimpl->timelog.log("Total", _pimpl->timer.time());
	}
//
	void VulkanWin32GraphicsSystem::SwapBuffer()
	{

		if (instance_->extent.height * instance_->extent.width == 0)
			return; //Skip swapping if there was nothing to render
		try
		{
			instance_->PresentFrame2();
		}
		catch (vk::Error& err)
		{
			LOG_CRASH_TO(LogPool::GFX, "Vulkan Error in swap buffer: %s", err.what());
			throw;//Rethrow
		}
	}
	void VulkanWin32GraphicsSystem::Shutdown()
	{
		instance_->View().Device()->waitIdle();
		_frame_renderers.clear();
		_debug_renderer->Shutdown();

		this->_pm.reset();
		_pimpl.reset();
		instance_.reset();
		ShFinalize();
	}
	GraphicsAPI VulkanWin32GraphicsSystem::GetAPI()
	{
		return GraphicsAPI::Vulkan;
	}
	void VulkanWin32GraphicsSystem::Prerender()
	{
		try {

		if(instance_->View().CurrFrame()==1)
			if (!_pimpl->rendered_brdf)
				RenderBRDF(renderer_fragment_shaders[FBrdf]);
		}
		catch (vk::SystemError& err)
		{
			LOG_CRASH_TO(LogPool::GFX, "Vulkan Error in prerender: %s", err.what());
			throw;//Rethrow
		}
		catch (std::exception& e)
		{
			LOG_CRASH_TO(LogPool::GFX, "Exception: in prerender %s", e.what());
			throw;//Rethrow
		}
		catch (...)
		{
			LOG_CRASH_TO(LogPool::GFX, "Unknown exception thrown in prerender");
			throw;//Rethrow
		}
	}
	dbg::time_log& VulkanWin32GraphicsSystem::TimeLog()
	{
		return _pimpl->timelog;
	}
	AsyncTexLoader& VulkanWin32GraphicsSystem::GetAsyncTexLoader()
	{
		return _pimpl->tex_loader;
	}
	VulkanState& VulkanWin32GraphicsSystem::GetVulkanHandle()
	{
		return *(instance_.get());
	}

	void VulkanWin32GraphicsSystem::SkeletonToUniforms(const SkeletonTransforms& trf, hash_table<string, string>& out)
	{
		
		auto& buffer = trf.bones_transforms;
		out["BoneMat4Block"] = string{ reinterpret_cast<const char*>(buffer.data()),hlp::buffer_size(buffer)};
	}


	/*
	void VulkanWin32GraphicsSystem::RenderGraphicsState(const GraphicsState& state, RenderStateV2& rs)
	{
		auto& swapchain = instance_->View().Swapchain();
		auto dispatcher = vk::DispatchLoaderDefault{};
		vk::CommandBuffer& cmd_buffer = rs.cmd_buffer;
		vk::CommandBufferInheritanceInfo aaa{};
		vk::CommandBufferBeginInfo begin_info{vk::CommandBufferUsageFlagBits::eOneTimeSubmit,&aaa};
		cmd_buffer.begin(begin_info, dispatcher);
		std::array<float, 4> a{};
		//TODO grab the appropriate framebuffer and begin renderpass
		vk::ClearValue v{ vk::ClearColorValue{r_cast<const std::array<float,4>&>(state.camera.clear_color)} };
		auto sz = instance_->View().GetWindowsInfo().size;
		vk::Rect2D render_area
		{
			vk::Offset2D{},vk::Extent2D
			{
				s_cast<uint32_t>(sz.x),s_cast<uint32_t>(sz.y)
			}
		};
		vk::RenderPassBeginInfo rpbi
		{
			*instance_->View().Renderpass(), *swapchain.frame_buffers[swapchain.curr_index],
			render_area,1,&v
		};
		cmd_buffer.beginRenderPass(rpbi, vk::SubpassContents::eInline, dispatcher);
		for (auto& obj : state.mesh_render)
		{
			rs.FlagRendered();
			//TODO Grab everything and render them
			auto& mesh = obj.mesh.as<VulkanMesh>();
			//auto& mat = obj.material_instance.material.as<VulkanMaterial>();
			thing.pipeline.Bind(cmd_buffer, instance_->View());
			auto& bindings =obj.attrib_bindings;
			for (auto&& [bindingz,attrib] : bindings)
			{
				auto& attrib_buffer = mesh.Get(attrib);
				cmd_buffer.bindVertexBuffers(bindingz, *attrib_buffer.buffer, vk::DeviceSize{ 0 }, vk::DispatchLoaderDefault{});
			}
			auto& oidx = mesh.GetIndexBuffer();
			if (oidx)
			{
				cmd_buffer.bindIndexBuffer(*(*oidx).buffer, 0, vk::IndexType::eUint16, vk::DispatchLoaderDefault{});
				cmd_buffer.drawIndexed(mesh.IndexCount(), 1, 0, 0, 0, vk::DispatchLoaderDefault{});
			}
		}
		cmd_buffer.endRenderPass();
		cmd_buffer.end();
	}*/

	/*void VulkanWin32GraphicsSystem::createInstance()
	{
		const char* extensions[] = {
			VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
			VK_KHR_SURFACE_EXTENSION_NAME,
			VK_KHR_EXTERNAL_MEMORY_CAPABILITIES_EXTENSION_NAME,
			VK_KHR_EXTERNAL_SEMAPHORE_CAPABILITIES_EXTENSION_NAME,
			VK_KHR_EXTERNAL_FENCE_CAPABILITIES_EXTENSION_NAME,
			VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME,
			VK_KHR_GET_SURFACE_CAPABILITIES_2_EXTENSION_NAME,
			VK_KHR_DEVICE_GROUP_CREATION_EXTENSION_NAME,
			VK_EXT_DEBUG_REPORT_EXTENSION_NAME,
			VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
		};

		vk::ApplicationInfo appInfo; 
		appInfo.setPApplicationName("Hello Triangle");
		appInfo.setApplicationVersion(VK_MAKE_VERSION(1, 0, 0));
		appInfo.setPEngineName("No Engine");
		appInfo.setEngineVersion(VK_MAKE_VERSION(1, 0, 0));
		appInfo.setApiVersion(VK_API_VERSION_1_0);
		
		vk::InstanceCreateInfo createInfo;
		createInfo.setPApplicationInfo(&appInfo);
		createInfo.setPpEnabledExtensionNames(extensions);
		createInfo.setEnabledExtensionCount((uint32_t) std::size(extensions));
		createInfo.setEnabledLayerCount(0);

		instance = vk::createInstanceUnique(createInfo);
	}*/
	/*
	void VulkanWin32GraphicsSystem::setupDebugMessenger()
	{
		vk::DebugUtilsMessengerCreateInfoEXT createInfo;
		createInfo.setMessageSeverity(vk::DebugUtilsMessageSeverityFlagBitsEXT::eError | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose);
		createInfo.setMessageType(vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation);
		createInfo.setPfnUserCallback(::debugCallback);

		VkDebugUtilsMessengerEXT tmp;
		CreateDebugUtilsMessengerEXT(*instance, createInfo, 0, &tmp);
		debugMessenger = VkHandle<vk::DebugUtilsMessengerEXT, vk::DispatchLoaderDynamic>{ tmp };
	}
	*/
	void DoNothing();
	namespace wtfareyoudoing
	{
		static int a = 0;
	}
//#pragma optimize("",off)
	void profile_bp_start()
	{
		wtfareyoudoing::a++;
		DoNothing();
	}
	void profile_bp_end()
	{
		DoNothing();
	}
}