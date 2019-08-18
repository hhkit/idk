#include "pch.h"
#include "VulkanDetail.h"
#include "Vulkan.h"
namespace vgfx
{

    vk::DispatchLoaderDefault& VulkanDetail::Dispatcher() const                     { return vulkan_.dispatcher      ;}
																												     
	vk::DispatchLoaderDynamic& VulkanDetail::DynDispatcher()const                   { return vulkan_.dyn_dispatcher  ;}
	vk::UniqueInstance&        VulkanDetail::Instance()const                        { return vulkan_.instance        ;}
	vk::UniqueSurfaceKHR             &VulkanDetail::Surface()const                  { return vulkan_.m_surface       ;}
	vk::PhysicalDevice               &VulkanDetail::PDevice()const                  { return vulkan_.pdevice         ;}
	vk::UniqueDevice                 &VulkanDetail::Device()const                   { return vulkan_.m_device        ;}
	QueueFamilyIndices& VulkanDetail::QueueFamily()const                            { return vulkan_.m_queue_family  ;}
	vk::Queue&          VulkanDetail::GraphicsQueue()const                          { return vulkan_.m_graphics_queue;}
	vk::Queue&          VulkanDetail::PresentQueue()const                           { return vulkan_.m_present_queue ;}
	//vk::Queue          m_transfer_queue = {}{}					                  				 ;
	SwapChainInfo                    & VulkanDetail::Swapchain()const               { return vulkan_.m_swapchain     ;}
																				      				 			     
	vk::UniqueRenderPass                 &VulkanDetail::Renderpass         ()const  { return vulkan_.m_renderpass    ;}
	vk::UniquePipeline                   &VulkanDetail::Pipeline           ()const  { return vulkan_.m_pipeline      ;}
	vk::UniqueCommandPool                &VulkanDetail::Commandpool        ()const  { return vulkan_.m_commandpool   ;}
																					  				 
	vk::UniqueCommandBuffer              &VulkanDetail::CurrCommandbuffer  ()const  { return vulkan_.m_commandbuffers[vulkan_.current_frame];}
	std::vector<vk::UniqueCommandBuffer> &VulkanDetail::Commandbuffers     ()const  { return vulkan_.m_commandbuffers;}

	inline vk::UniqueShaderModule VulkanDetail::CreateShaderModule(const idk::string_view& code)
	{
		vk::ShaderModuleCreateInfo mod{
			vk::ShaderModuleCreateFlags{},
			code.length(),reinterpret_cast<uint32_t const*>(code.data())
		};
		return Device()->createShaderModuleUnique(mod);
	}

	VulkanDetail::VulkanDetail(::Vulkan& vulkan) :vulkan_{ vulkan } {}
}