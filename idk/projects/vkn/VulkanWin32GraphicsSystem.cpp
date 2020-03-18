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
		RscHandle<Texture> BrdfLookupTable;
	};

	VulkanWin32GraphicsSystem::VulkanWin32GraphicsSystem() :  instance_{ std::make_unique<VulkanState>() }
	{
		if (!instance_)
			throw;
		assert(instance_);
	}
	VulkanWin32GraphicsSystem::~VulkanWin32GraphicsSystem()
	{
	}
	void VulkanWin32GraphicsSystem::Init()
	{
		windows_ = &Core::GetSystem<win::Windows>();
		instance_->InitVulkanEnvironment(window_info{ windows_->GetScreenSize(),windows_->GetWindowHandle(),windows_->GetInstance() });
		windows_->OnScreenSizeChanged.Listen([this](const ivec2&) { Instance().OnResize(); });

		if (!instance_)
			throw;
		RegisterFactories();
		_pm = std::make_unique<PipelineManager>();

		_pimpl = std::make_unique<Pimpl>();
		_pimpl->allocator = std::make_unique<hlp::MemoryAllocator>(*instance_->View().Device(), instance_->View().PDevice());
		_pimpl->fence = instance_->View().Device()->createFenceUnique({});
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
		GraphicsSystem::LateInit();
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

	namespace hlp
	{
		string DumpAllocators();
		std::pair<size_t, size_t> DumpAllocator(std::ostream& out, const MemoryAllocator& alloc);
	}
	void ReloadDeferredShaders()
	{
		Core::GetSystem<GraphicsSystem>().LoadShaders();
	}
	void VulkanWin32GraphicsSystem::RenderRenderBuffer()
	{
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

		auto& curr_signal = instance_->View().CurrPresentationSignals();
		instance_->AcquireFrame(*curr_signal.image_available);
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
		auto& curr_frame = _frame_renderers[curr_index];
		auto& curr_buffer = object_buffer[curr_draw_buffer];
		_pm->CheckForUpdates(curr_index);

		std::vector<GraphicsState> curr_states(curr_buffer.camera.size());

		_debug_renderer->GrabDebugBuffer();

		SharedGraphicsState& shared_graphics_state=curr_frame.shared_graphics_state;
		shared_graphics_state.Reset();
		auto& lights = curr_buffer.lights;
		shared_graphics_state.mat_inst_cache = &curr_frame.GetMatInstCache();
		shared_graphics_state.Init(lights,curr_buffer.instanced_mesh_render);
		shared_graphics_state.BrdfLookupTable = _pimpl->BrdfLookupTable;
		shared_graphics_state.particle_data = &curr_buffer.particle_buffer;
		shared_graphics_state.particle_range = &curr_buffer.particle_range;

		shared_graphics_state.characters_data = &curr_buffer.font_buffer;
		shared_graphics_state.fonts_data = &curr_buffer.font_render_data;
		shared_graphics_state.font_range = &curr_buffer.font_range;

		shared_graphics_state.ui_render_per_canvas = &curr_buffer.ui_render_per_canvas;
		shared_graphics_state.ui_canvas = &curr_buffer.ui_canvas;
		shared_graphics_state.ui_text_data = &curr_buffer.ui_text_buffer;
		shared_graphics_state.ui_text_range = &curr_buffer.ui_text_range;
		shared_graphics_state.total_num_of_text = curr_buffer.ui_total_num_of_text;
		//shared_graphics_state.ui_canvas_range = &curr_buffer.ui_canvas_range;

		PreRenderData pre_render_data;
		pre_render_data.shared_gfx_state = &shared_graphics_state;
		pre_render_data.active_lights.reserve(lights.size());
		pre_render_data.active_dir_lights.reserve(curr_buffer.directional_light_buffer.size());
		pre_render_data.cameras = &curr_buffer.camera;
		pre_render_data.d_lightmaps = &curr_buffer.d_lightmaps;

		for (size_t i = 0; i < lights.size(); ++i)
			if(lights[i].cast_shadow && lights[i].index!=0)
				pre_render_data.active_lights.emplace_back(i);

		for (auto& elem : curr_buffer.directional_light_buffer)
			pre_render_data.active_dir_lights.emplace_back(elem);

		pre_render_data.Init(curr_buffer.mesh_render, curr_buffer.skinned_mesh_render, curr_buffer.skeleton_transforms,curr_buffer.inst_mesh_render_buffer);
		if (&curr_buffer.skeleton_transforms != pre_render_data.skeleton_transforms)
			throw;

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
		//for (auto& camera : curr_buffer.camera)
		//{
		//	/*auto& pimpl = _pimpl;
		//	std::visit([&](auto& clear)
		//		{
		//			if constexpr (std::is_same_v<std::decay_t<decltype(clear)>, RscHandle<CubeMap>>)
		//			{
		//				const RscHandle<CubeMap>& cubemap = clear;
		//				if (!cubemap)
		//					return;
		//				VknCubemap& cm = cubemap.as<VknCubemap>();
		//				RscHandle<VknCubemap> conv = cm.GetConvoluted();
		//				if (RscHandle < VknCubemap>{} == conv)
		//				{
		//					conv = Core::GetResourceManager().Create<VknCubemap>();
		//					conv->Size(cubemap->Size());
		//					CubemapLoader loader;
		//					CubemapOptions options{cm.GetMeta()};
		//					CMCreateInfo info = CMColorBufferTexInfo(cubemap->Size().x, cubemap->Size().y);
		//					info.image_usage |= vk::ImageUsageFlagBits::eColorAttachment;
		//					loader.LoadCubemap(conv.as<VknCubemap>(), *pimpl->allocator, *pimpl->fence, options, info, {});
		//					cm.SetConvoluted(conv);
		//				}
		//			}
		//		}, camera.clear_data);*/
		//}
		bool will_draw_debug = true;
		for (size_t i = 0; i < curr_states.size()&&i<curr_buffer.culled_render_range.size(); ++i)
		{
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
			curr_state.ProcessMaterialInstances(shared_graphics_state.material_instances, curr_frame.GetMatInstCache());
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
			//_debug_renderer->Render(curr_state.camera.view_matrix, mat4{1,0,0,0,   0,-1,0,0,   0,0,0.5f,0.5f, 0,0,0,1}*curr_state.camera.projection_matrix);
		}
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
		if (RscHandle<VknRenderTarget>{}->NeedsFinalizing())
			RscHandle<VknRenderTarget>{}->Finalize();
		// */
		//string test = hlp::DumpAllocators();

		


		curr_frame.ColorPick(std::move(request_buffer));
		curr_frame.PreRenderGraphicsStates(pre_render_data, curr_index); //TODO move this to Prerender
#if 0
		{
			auto str = dbg::DumpFrameBufferAllocs();
		}
#endif
		//std::reverse(curr_states.begin(), curr_states.end());
		curr_frame.RenderGraphicsStates(curr_states, curr_index);
#if 0
		{
			auto str = dbg::DumpFrameBufferAllocs();
		}
#endif
		curr_frame.PostRenderGraphicsStates(post_render_data, curr_index);


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

		if (extra_vars.GetOptional<bool>("Dump", false))
		{
			extra_vars.Set("Dump", false);
			dump();
		}

		}
		catch (vk::SystemError& err)
		{
			LOG_CRASH_TO(LogPool::GFX, "Vulkan Error: %s", err.what());
			dump();
			throw;
		}
	}
//
	void VulkanWin32GraphicsSystem::SwapBuffer()
	{
		try
		{
			instance_->PresentFrame2();
		}
		catch (vk::SystemError& err)
		{
			LOG_CRASH_TO(LogPool::GFX, "Vulkan Error: %s", err.what());
			throw;
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
		if(instance_->View().CurrFrame()==1)
			if (!_pimpl->rendered_brdf)
				RenderBRDF(renderer_fragment_shaders[FBrdf]);
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
}