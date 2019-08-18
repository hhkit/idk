#pragma once
#include <vulkan/vulkan.hpp>
#include "Vulkan.h"
namespace vgfx
{
	//Interface for the vulkan details. 
	//Ideally we should move all of the vk:: related data and into VulkanDetail
	//Effectively PIMPLing the vk stuff
	class VulkanDetail
	{
	public:
		vk::DispatchLoaderDefault& Dispatcher()const;
		vk::DispatchLoaderDynamic& DynDispatcher()const;
		vk::UniqueInstance&        Instance()const;
		vk::UniqueSurfaceKHR             &Surface()const;
		vk::PhysicalDevice               &PDevice()const;
		vk::UniqueDevice                 &Device()const;
		QueueFamilyIndices& QueueFamily()const;
		vk::Queue&          GraphicsQueue()const;
		vk::Queue&          PresentQueue()const;
		//vk::Queue          m_transfer_queue = {};
		SwapChainInfo                    &Swapchain()const;

		vk::UniqueRenderPass                 &Renderpass         ()const;
		vk::UniquePipeline                   &Pipeline           ()const;
		vk::UniqueCommandPool                &Commandpool        ()const;

		vk::UniqueCommandBuffer              &CurrCommandbuffer  ()const;
		std::vector<vk::UniqueCommandBuffer> &Commandbuffers     ()const;



		vk::UniqueShaderModule  CreateShaderModule(const idk::string_view& code);


		VulkanDetail(::Vulkan& vulkan);
		VulkanDetail(VulkanDetail&&) = default;
		VulkanDetail& operator=(VulkanDetail&&) = default;
	private:
		::Vulkan& vulkan_;
	};
}