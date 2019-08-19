#pragma once
#include <vulkan/vulkan.hpp>
#include <vulkan/vkn.h>
namespace vgfx
{
	struct RenderState;
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

		vk::UniquePipeline                   &Pipeline           ()const;
		vk::UniqueCommandPool                &Commandpool        ()const;
		std::vector<vk::UniqueCommandBuffer> &Commandbuffers     ()const;

		//Render State info
		void                     SwapRenderState     ()const;//Probably a bad decision to const this shit.
		idk::vector<RenderState> &RenderStates       ()const;
		RenderState              &PrevRenderState    ()const;
		RenderState              &CurrRenderState    ()const;
		vk::UniqueRenderPass     &Renderpass         ()const;
		vk::UniqueCommandBuffer  &CurrCommandbuffer  ()const;
		vk::Buffer               &CurrMasterVtxBuffer()const;
		//Copies the data into the master buffer and returns the offset to start from.
		uint32_t                 AddToMasterBuffer  (const void* data, uint32_t len)const;
		void                     ResetMasterBuffer  ()const;



		vk::UniqueShaderModule  CreateShaderModule(const idk::string_view& code);


		VulkanDetail(::Vulkan& vulkan);
		VulkanDetail(VulkanDetail&&) noexcept;
		VulkanDetail& operator=(VulkanDetail&&) noexcept;
		//VulkanDetail(const VulkanDetail&)  = default;
		//VulkanDetail& operator=(const VulkanDetail&)  = default;
		~VulkanDetail();
	private:
		struct pimpl;
		std::unique_ptr<pimpl> impl_;
		::Vulkan& vulkan()const;
		::Vulkan* vulkan_;
	};
}