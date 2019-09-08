#include <pch.h>
#include <iostream>
#include <vkn/VulkanWin32GraphicsSystem.h>
#include <WindowsApplication.h>

#include <vkn/VulkanState.h>
#include <vkn/VulkanMesh.h>
#include <core/Core.h>

#include <vkn/VulkanPipeline.h>
#include <gfx/pipeline_config.h>
#include <file/FileSystem.h>
#include <vkn/VulkanMeshFactory.h>
#include <vkn/VulkanShaderModuleFactory.h>
#include <res/ForwardingExtensionLoader.h>
#include <vkn/VulkanGlslLoader.h>
#include <vkn/VulkanMaterialFactory.h>
#include <gfx/ShaderTemplateFactory.h>
#include <anim/SkeletonFactory.h>
#include <anim/AnimationFactory.h>
#include <vkn/PipelineManager.h>

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

	struct SomeHackyThing
	{
		VulkanPipeline pipeline;
	};
	static SomeHackyThing thing;
	VulkanWin32GraphicsSystem::VulkanWin32GraphicsSystem() :  instance_{ std::make_unique<VulkanState>() }
	{
	}
	void VulkanWin32GraphicsSystem::Init()
	{
		windows_ = &Core::GetSystem<win::Windows>();
		instance_->InitVulkanEnvironment(window_info{ windows_->GetScreenSize(),windows_->GetWindowHandle(),windows_->GetInstance() });

		Core::GetResourceManager().RegisterFactory<ShaderTemplateFactory>();
		Core::GetResourceManager().RegisterFactory<idk::anim::SkeletonFactory>();
		Core::GetResourceManager().RegisterFactory<idk::anim::AnimationFactory>();
		Core::GetResourceManager().RegisterFactory<MeshFactory>();
		Core::GetResourceManager().RegisterFactory<VulkanMaterialFactory>();
		Core::GetResourceManager().RegisterFactory<VulkanGlslLoader>();
		//Core::GetResourceManager().RegisterFactory<VulkanMaterialFactory>();
		//Core::GetResourceManager().RegisterExtensionLoader<ForwardingExtensionLoader<Material>>(".frag");
		//Core::GetResourceManager().RegisterExtensionLoader<ForwardingExtensionLoader<ShaderProgram>>(".fragspv");
		//Core::GetResourceManager().RegisterExtensionLoader<ForwardingExtensionLoader<ShaderProgram>>(".vertspv");
		Core::GetResourceManager().RegisterExtensionLoader<ForwardingExtensionLoader<ShaderProgram>>(".vert");
		Core::GetResourceManager().RegisterExtensionLoader<ForwardingExtensionLoader<ShaderProgram>>(".frag");
		Core::GetResourceManager().RegisterExtensionLoader<ForwardingExtensionLoader<ShaderTemplate>>(".tmpt");
		_pm = std::make_unique<PipelineManager>();
		_pm->View(instance_->View());
	}
	void VulkanWin32GraphicsSystem::LateInit()
	{
		_frame_renderers.resize(instance_->View().Swapchain().frame_objects.size());
		for (auto& frame : _frame_renderers)
		{
			frame.Init(&instance_->View(), *instance_->View().Commandpool());
			frame.SetPipelineManager(*_pm);
		}
	}
	void VulkanWin32GraphicsSystem::RenderRenderBuffer()
	{
		auto& curr_signal = instance_->View().CurrPresentationSignals();
		instance_->AcquireFrame(*curr_signal.image_available);
		auto curr_index = instance_->View().Swapchain().curr_index;
		auto& curr_frame = _frame_renderers[curr_index];
		auto& curr_buffer = object_buffer[curr_draw_buffer];
		_pm->CheckForUpdates(curr_index);
		std::vector<GraphicsState> curr_states(1);// curr_buffer.camera.size());
		for (size_t i = 0; i < curr_states.size(); ++i)
		{
			auto& curr_state = curr_states[i];
			curr_state.Init(curr_buffer.camera[i], curr_buffer.mesh_render,curr_buffer.skinned_mesh_render);
		}
		// */
		curr_frame.RenderGraphicsStates(curr_states);
		instance_->DrawFrame(*curr_frame.GetMainSignal().render_finished,*curr_signal.render_finished);
	}
	void VulkanWin32GraphicsSystem::SwapBuffer()
	{
		using namespace std::chrono_literals;
		static std::optional<std::chrono::time_point<std::chrono::high_resolution_clock>> last_time{};

		if (last_time)
		{
			;
			while ((std::chrono::high_resolution_clock::now() - *last_time) < (1000ms / 60));
		}
		last_time = std::chrono::high_resolution_clock::now();
		instance_->PresentFrame(*instance_->View().CurrPresentationSignals().render_finished);
	}
	void VulkanWin32GraphicsSystem::BeginFrame()
	{
		instance_->BeginFrame();
	}
	void VulkanWin32GraphicsSystem::EndFrame()
	{
		instance_->EndFrame();
	}
	void VulkanWin32GraphicsSystem::Shutdown()
	{
		_frame_renderers.clear();
		instance_.reset();
	}
	GraphicsAPI VulkanWin32GraphicsSystem::GetAPI()
	{
		return GraphicsAPI::Vulkan;
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