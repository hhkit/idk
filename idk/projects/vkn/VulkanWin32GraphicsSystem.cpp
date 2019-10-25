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

	VulkanWin32GraphicsSystem::VulkanWin32GraphicsSystem() :  instance_{ std::make_unique<VulkanState>() }
	{
	}
	VulkanWin32GraphicsSystem::~VulkanWin32GraphicsSystem()
	{
	}
	void VulkanWin32GraphicsSystem::Init()
	{
		windows_ = &Core::GetSystem<win::Windows>();
		instance_->InitVulkanEnvironment(window_info{ windows_->GetScreenSize(),windows_->GetWindowHandle(),windows_->GetInstance() });
		windows_->OnScreenSizeChanged.Listen([this](const ivec2&) { Instance().OnResize(); });

		RegisterFactories();
		_pm = std::make_unique<PipelineManager>();
		_pm->View(instance_->View());

	}
	void VulkanWin32GraphicsSystem::LateInit()
	{
		GraphicsSystem::LateInit();
		_debug_renderer = std::make_unique<VulkanDebugRenderer>();
		_debug_renderer->Init();
		_frame_renderers.resize(instance_->View().Swapchain().frame_objects.size());
		for (auto& frame : _frame_renderers)
		{
			frame.Init(&instance_->View(), *instance_->View().Commandpool());
			frame.SetPipelineManager(*_pm);
		}
		instance_->imguiEnabled = s_cast<bool>(&Core::GetSystem<IEditor>());
		TestFunc();
	}
	/*
	vector<IBufferedObj*> GetBufferedResources(vector<GraphicsState>& states)
	{
		hash_set<RscHandle<Texture>> textures;
		hash_set<RscHandle<Mesh>> meshes;
		for (auto& state : states)
		{
			for (auto& mesh_render : state.mesh_render)
			{
				meshes.emplace(mesh_render->mesh);
				auto& m_inst = mesh_render->material_instance;
				for (auto& uniform : m_inst.uniforms)
				{
					if (m_inst.IsImageBlock(uniform.first))
					{
						for (auto& tex : m_inst.GetImageBlock(uniform.first))
						{
							textures.emplace(tex.second);
						}
					}
				}	
			}
		}

		//Transform the result sets into buffered object vector
		vector<IBufferedObj*> objs;
		for (auto& mesh : meshes)
		{

		}
	}
	
	void UpdateResources(uint32_t frame_index, const vector<IBufferedObj*>& buffered_objects)
	{
		for (auto p_obj : buffered_objects)
		{
			p_obj->UpdateCurrent(frame_index);
		}
	}
	*/
	void VulkanWin32GraphicsSystem::RenderRenderBuffer()
	{
		auto& curr_signal = instance_->View().CurrPresentationSignals();
		instance_->AcquireFrame(*curr_signal.image_available);
		auto curr_index = instance_->View().CurrFrame();
		instance_->ResourceManager().ProcessQueue(curr_index);
		auto& curr_frame = _frame_renderers[curr_index];
		auto& curr_buffer = object_buffer[curr_draw_buffer];
		_pm->CheckForUpdates(curr_index);

		//auto copy_cams = curr_buffer.light_camera_data;
		//std::copy(curr_buffer.camera.begin(), curr_buffer.camera.end(), std::back_inserter(copy_cams));
		//std::swap(copy_cams, curr_buffer.camera);

		std::vector<GraphicsState> curr_states(curr_buffer.camera.size());

		_debug_renderer->GrabDebugBuffer();

		SharedGraphicsState shared_graphics_state;
		auto& lights = curr_buffer.lights;
		shared_graphics_state.Init(lights);

		PreRenderData pre_render_data;
		pre_render_data.shared_gfx_state = &shared_graphics_state;
		pre_render_data.active_lights.resize(lights.size());

		//TODO cull the unused lights
		for (size_t i = 0; i < lights.size(); ++i)
			pre_render_data.active_lights[i]=i;

		pre_render_data.Init(curr_buffer.mesh_render, curr_buffer.skinned_mesh_render, curr_buffer.skeleton_transforms);
		pre_render_data.mesh_vtx = curr_buffer.mesh_vtx;
		pre_render_data.skinned_mesh_vtx = curr_buffer.skinned_mesh_vtx;
		hash_set<RscHandle<RenderTarget>> render_targets;

		auto IsDontClear = [](const CameraData& camera)
		{
			return camera.clear_data.index() == meta::IndexOf <std::remove_const_t<decltype(camera.clear_data)>, DontClear>::value;
		};
		for (size_t i = 0; i < curr_states.size(); ++i)
		{
			auto& curr_state = curr_states[i];
			auto& curr_cam = curr_buffer.camera[i];
			curr_state.Init(curr_cam, curr_buffer.lights, curr_buffer.mesh_render, curr_buffer.skinned_mesh_render,curr_buffer.skeleton_transforms);
			const auto itr = render_targets.find(curr_cam.render_target);
			//const bool new_rt = 
				curr_state.clear_render_target = !IsDontClear(curr_cam);

			if(itr==render_targets.end())
				render_targets.emplace(curr_cam.render_target);

			curr_state.mesh_vtx = curr_buffer.mesh_vtx;
			curr_state.skinned_mesh_vtx = curr_buffer.skinned_mesh_vtx;
			curr_state.dbg_render.resize(0);
			curr_state.shared_gfx_state = &shared_graphics_state;
			if (curr_cam.overlay_debug_draw)
			{
				curr_state.dbg_pipeline = &_debug_renderer->GetPipeline();
				//TODO Add cull step
				curr_state.dbg_render.reserve(_debug_renderer->DbgDrawCalls().size());
				for (auto& dbgcall : _debug_renderer->DbgDrawCalls())
				{
					curr_state.dbg_render.emplace_back(&dbgcall);
				}
			}
			//_debug_renderer->Render(curr_state.camera.view_matrix, mat4{1,0,0,0,   0,-1,0,0,   0,0,0.5f,0.5f, 0,0,0,1}*curr_state.camera.projection_matrix);
		}

		for (auto& prt : render_targets)
		{
			if (prt->NeedsFinalizing())
				prt->Finalize();
		}
		// */
		curr_frame.PreRenderGraphicsStates(pre_render_data, curr_index); //TODO move this to Prerender
		curr_frame.RenderGraphicsStates(curr_states, curr_index);
		instance_->DrawFrame(*curr_frame.GetMainSignal().render_finished,*curr_signal.render_finished);
	}
	void VulkanWin32GraphicsSystem::SwapBuffer()
	{
		//using namespace std::chrono_literals;
		//static std::optional<std::chrono::time_point<std::chrono::high_resolution_clock>> last_time{};
		//
		//if (last_time)
		//{
		//	;
		//	while ((std::chrono::high_resolution_clock::now() - *last_time) < (1000ms / 60));
		//}
		//last_time = std::chrono::high_resolution_clock::now();
		instance_->PresentFrame2();
	}
	void VulkanWin32GraphicsSystem::Shutdown()
	{
		_debug_renderer->Shutdown();

		this->_pm.reset();
		_frame_renderers.clear();
		instance_.reset();
		ShFinalize();
	}
	GraphicsAPI VulkanWin32GraphicsSystem::GetAPI()
	{
		return GraphicsAPI::Vulkan;
	}
	void VulkanWin32GraphicsSystem::Prerender()
	{
	}
	VulkanState& VulkanWin32GraphicsSystem::GetVulkanHandle()
	{
		return *(instance_.get());
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