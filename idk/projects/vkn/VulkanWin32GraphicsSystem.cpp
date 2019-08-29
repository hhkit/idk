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
		

		uint32_t pos_loc = 0;
		uint32_t nml_loc = 1;
		uint32_t pos_binding = 0;
		uint32_t nml_binding = 1;
		uint32_t zzz_binding = 2;
		pipeline_config config{};
		buffer_desc pos_desc{};
		buffer_desc nml_desc{};
		
		pos_desc.AddAttribute(AttribFormat::eSVec3, pos_loc, 0);
		nml_desc.AddAttribute(AttribFormat::eSVec3, nml_loc, 0);
		pos_desc.binding.binding_index = pos_binding;
		pos_desc.binding.stride = 0;
		pos_desc.binding.vertex_rate = eVertex;
		nml_desc.binding.binding_index = nml_binding;
		nml_desc.binding.stride = 0;
		nml_desc.binding.vertex_rate = eVertex;

		config.buffer_descriptions.emplace_back(pos_desc);
		config.buffer_descriptions.emplace_back(nml_desc);
		string f, v;
		{
			std::stringstream stringify;
			{
				auto vbuffer = Core::GetSystem<FileSystem>().Open("/assets/shader/mesh.vert", FS_PERMISSIONS::READ, false);
				stringify << vbuffer.rdbuf();
			}
			v = stringify.str();
			config.vert_shader = v;

		}
		{
			std::stringstream stringify;
			{
				auto vbuffer = Core::GetSystem<FileSystem>().Open("/assets/shader/flat_color.frag", FS_PERMISSIONS::READ, false);
				stringify << vbuffer.rdbuf();
			}
			f = stringify.str();
			config.frag_shader = f;
		}
		config.prim_top = PrimitiveTopology::eTriangleList;
		thing.pipeline.Create(config, instance_->View());

		uint32_t num_fo = instance_->View().Swapchain().frame_objects.size();
		uint32_t num_concurrent_states = 1;
		frames.resize(num_fo);
		auto cmd_pool = *instance_->View().Commandpool();
		auto device = *instance_->View().Device();
		auto& buffers = cmd_buffers = device.allocateCommandBuffersUnique(vk::CommandBufferAllocateInfo{ cmd_pool,vk::CommandBufferLevel::eSecondary, num_concurrent_states*num_fo }, vk::DispatchLoaderDefault{});
		pri_buffer= std::move(device.allocateCommandBuffersUnique(vk::CommandBufferAllocateInfo{ cmd_pool,vk::CommandBufferLevel::ePrimary, 1}, vk::DispatchLoaderDefault{})[0]);


		for (auto f = frames.size();f-->0;)
		{
			auto& frame = frames[f];
			for (auto i = num_concurrent_states; i-- > 0;)
			{
				frame.states.emplace_back(RenderStateV2{ *buffers[f * num_concurrent_states + i],instance_->View() });
			}
		}
	}
	void VulkanWin32GraphicsSystem::RenderRenderBuffer()
	{
		instance_->DrawFrame();
		auto& curr_buffer = object_buffer[curr_draw_buffer];
		auto& curr_frame = frames[instance_->View().Swapchain().curr_index];
		size_t num_concurrent = curr_frame.states.size();
		for (auto& state : curr_frame.states)
		{
			state.cmd_buffer.reset({}, vk::DispatchLoaderDefault{});
		}
		for (size_t i=0;i+num_concurrent<curr_buffer.states.size();i+=num_concurrent)
		{
			//Spawn/Assign to the threads
			for (size_t j = 0; j < num_concurrent; ++j){
				auto& state = curr_buffer.states[i + j];
				RenderGraphicsState(state,curr_frame.states[j]);//We may be able to multi thread this
			}
			//Wait here
		}
		pri_buffer->reset({}, vk::DispatchLoaderDefault{});
		vk::CommandBufferBeginInfo begin_info{ vk::CommandBufferUsageFlagBits::eOneTimeSubmit };
		pri_buffer->begin(begin_info, vk::DispatchLoaderDefault{});
		vector<vk::CommandBuffer> buffers{};
		for (auto& state : curr_frame.states)
		{
			buffers.emplace_back(state.cmd_buffer);
		}
		pri_buffer->executeCommands(buffers, vk::DispatchLoaderDefault{});
		pri_buffer->end();
		vk::SubmitInfo submit_info{};
		instance_->View().GraphicsQueue().submit(submit_info, vk::Fence{}, vk::DispatchLoaderDefault{});
	}
	void VulkanWin32GraphicsSystem::SwapBuffer()
	{
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



	void VulkanWin32GraphicsSystem::RenderGraphicsState(const GraphicsState& state, RenderStateV2& rs)
	{
		auto& swapchain = instance_->View().Swapchain();
		auto dispatcher = vk::DispatchLoaderDefault{};
		vk::CommandBuffer& cmd_buffer = rs.cmd_buffer;
		vk::CommandBufferBeginInfo begin_info{vk::CommandBufferUsageFlagBits::eOneTimeSubmit};
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
		cmd_buffer.end();
	}
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