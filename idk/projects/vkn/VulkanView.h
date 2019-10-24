#pragma once
#include <idk.h>
#include <vulkan/vulkan.hpp>
#include <vkn/vulkan_state_fwd.h>
namespace idk::vkn
{
	class VulkanState;
	class VulkanResourceManager;
	struct RenderState;
	//Interface for the vulkan details. 
	//Ideally we should move all of the vk:: related data and into VulkanDetail
	//Effectively PIMPLing the vk stuff
	class VulkanView
	{
	public:
		VulkanResourceManager&              ResourceManager()const;
		vk::DispatchLoaderDefault&          Dispatcher()const;
		vk::DispatchLoaderDynamic&          DynDispatcher()const;
		vk::UniqueInstance&                 Instance()const;
		vk::UniqueSurfaceKHR&               Surface()const;
		vk::PhysicalDevice&                 PDevice()const;
		uint32_t                            BufferOffsetAlignment()const;
		uint32_t                            BufferSizeAlignment()const;
		vk::UniqueDevice&                   Device()const;
		QueueFamilyIndices&                 QueueFamily()const;
		vk::Queue&                          GraphicsQueue()const;
		vk::Queue&                          PresentQueue()const;
		SwapChainInfo&                      Swapchain()const;
		uint32_t                            CurrFrame()const;
		PresentationSignals&   CurrPresentationSignals()const;

		vk::UniqueCommandPool&     Commandpool()const;

		//Render State info
		void                       SwapRenderState()const;//Probably a bad decision to const this shit.
		vector<RenderState>&       RenderStates()const;
		RenderState&               PrevRenderState()const;
		RenderState&               CurrRenderState()const;
		vk::UniqueRenderPass&      Renderpass()const;
		vk::UniqueRenderPass&      ContinuedRenderpass()const;
		vk::Buffer&                CurrMasterVtxBuffer()const;
		//Copies the data into the master buffer and returns the offset to start from.
		uint32_t                   AddToMasterBuffer  (const void* data, uint32_t len)const;
		void                       ResetMasterBuffer  ()const;
		bool&					   ImguiResize        ();
		window_info&			   GetWindowsInfo     ()const;
		PresentationSignals& GetCurrentSignals        ()const;
		uint32_t				   CurrSemaphoreFrame ()const;
		uint32_t				   AcquiredImageValue ()const;
		vk::RenderPass             BasicRenderPass    (BasicRenderPasses type, bool clear_col = true, bool clear_depth = true)const;
		vk::Result&				AcquiredImageResult()const;
		uint32_t				   MaxFrameInFlight()const;
		uint32_t                   SwapchainImageCount()const;

		VulkanState& vulkan() const;


		vk::UniqueShaderModule     CreateShaderModule(const string_view& code);

		void WaitDeviceIdle() const;

		VulkanView(VulkanState& vulkan);
		VulkanView(VulkanView&&) noexcept = default;
		VulkanView& operator=(VulkanView&&) noexcept = default;
		~VulkanView();
	private:
		struct pimpl;
		std::unique_ptr<pimpl> impl_;
		VulkanState* vulkan_;
	};
}