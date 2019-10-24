#include "pch.h"
#pragma once
#include <optional>
#include <fstream>
#include <sstream>
#pragma warning(disable:26495)
#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_win32.h>
#include <vkn/DescriptorsManager.h>
#include <vkn/UboManager.h>
#include <vkn/VknTexture.h>
#include <vkn/ValHandler.h>
#include <vkn/VulkanResourceManager.h>
#include <vkn/utils/PresentationSignals.h>
#include <vkn/utils/TriBuffer.h>

#include <vkn/utils/SwapchainInfo.h>
#include <vkn/vulkan_state_fwd.h>
#include <vkn/VulkanHashes.h>
#undef max
#undef min


namespace idk::vkn 
{
	
	inline BasicRenderPasses operator++(BasicRenderPasses& type)
	{
		return type = s_cast<BasicRenderPasses>(s_cast<uint32_t>(type) + 1);
	}
	inline BasicRenderPasses operator++(BasicRenderPasses& type, int)
	{
		auto result = type;
		++type;
		return result;
	}
	struct window_info
	{
		ivec2     size;
		HWND      wnd;
		HINSTANCE winstance;
	};

	class VulkanState;

	struct QueueFamilyIndices
	{
		uint32_t					 queueFamilyIndexCount;
		std::optional<uint32_t>      graphics_family;
		std::optional<uint32_t>      present_family;
		std::optional<uint32_t>      transfer_family;
		std::unordered_set<uint32_t> unique_queues()const;
		uint32_t*					 pQueueFamilyIndices;
		bool isComplete()const;
	};
	struct SwapChainSupportDetails {
		vk::SurfaceCapabilitiesKHR capabilities;
		std::vector<vk::SurfaceFormatKHR> formats;
		std::vector<vk::PresentModeKHR> presentModes;
	};

	typedef uint32_t renderFrameIndex;

	class VulkanView;

	class VulkanState
	{
	public:
		using QueueFamilyIndices     = QueueFamilyIndices;
		using SwapChainInfo          = SwapChainInfo;
		using SwapChainSupportDetails= SwapChainSupportDetails;
		struct vbo;//vertex buffer object
		struct ubo;//uniform buffer object
		struct pipeline;
		struct pipeline_config;

		using unique_vbo      = unique_ptr<vbo     >;
		using unique_ubo      = unique_ptr<ubo     >;
		using unique_pipeline = unique_ptr<pipeline>;


		VulkanState();
		~VulkanState();

		VulkanResourceManager& ResourceManager();

		void UpdateWindowSize(vec2 size);
		void CleanupSwapChain();
		void RecreateSwapChain();

		void InitVulkanEnvironment(window_info info);
		void NextFrame();
		uint32_t MaxFramesInFlight()const { return max_frames_in_flight; }

		VulkanView& View();

		void AcquireFrame(vk::Semaphore signal);
		void DrawFrame(vk::Semaphore wait, vk::Semaphore signal);
		void PresentFrame(vk::Semaphore wait);
		void PresentFrame2();
		void OnResize();
		void Cleanup();

		//renderFrameIndex GetRenderFrameIndex();
		//void SubmitRenderFrameInfo(const renderFrameIndex& );

		void createImage(uint32_t width, uint32_t height, vk::Format format, vk::ImageTiling tiling, vk::ImageUsageFlags usage, vk::MemoryPropertyFlags properties, vk::Image& image, hlp::MemoryAllocator&);


		static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
			VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
			VkDebugUtilsMessageTypeFlagsEXT messageType,
			const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
			void* pUserData);
		bool						imguiEnabled{ true };

#pragma region ("Render passes")
		vk::RenderPass BasicRenderPass(BasicRenderPasses type, bool clear_col , bool clear_depth )const;
		//vk::RenderPass RenderPass_RgbaColorOnly ()const;
		//vk::RenderPass RenderPass_DepthOnly     ()const;
		//vk::RenderPass RenderPass_RgbaColorDepth()const;
#pragma endregion


		uint32_t					imageCount;
		vk::Extent2D                extent;
		vk::Format					format;
		vk::SurfaceFormatKHR        surfaceFormat;

	private:
		// type aliases
		friend class VulkanView;
		template<typename T>
		using DynamicHandle = vk::UniqueHandle<T, vk::DispatchLoaderDynamic>;
		// variables


		// init info
		vk::UniqueInstance               instance;
		vk::PhysicalDevice               pdevice = {};
		uint32_t                         buffer_offset_alignment = {};
		uint32_t                         buffer_size_alignment = {};
		vk::UniqueDevice                 m_device;
		QueueFamilyIndices               m_queue_family = {};
		vk::Queue                        m_graphics_queue = {};
		vk::Queue                        m_present_queue = {};
		vk::UniqueDescriptorPool         m_descriptorpool;

		vk::DispatchLoaderDefault        dispatcher = {};
		vk::DispatchLoaderDynamic        dyn_dispatcher = {};
		DynamicHandle<vk::DebugUtilsMessengerEXT> m_debug_messenger;


		window_info m_window;
		bool m_ScreenResized = false;
		bool m_ScreenResizedForImGui = false;
		uint32_t WIDTH = 1280, HEIGHT = 720;
		uint32_t current_frame = 0, max_frames_in_flight = 2;
		vk::UniqueSurfaceKHR                 m_surface;



		vk::UniqueCommandPool                m_commandpool;
		std::vector<vk::UniqueCommandBuffer> m_pri_commandbuffers;
		std::vector<vk::UniqueCommandBuffer> m_present_trf_commandbuffers;
		vector<vk::UniqueCommandBuffer> m_blitz_commandbuffers;
		
		//Deprecated
		//std::vector<PresentationSignals>     m_pres_signals;

		std::unique_ptr<SwapChainInfo>       m_swapchain;



		vk::UniqueRenderPass                 m_renderpass;
		vk::UniqueRenderPass                 m_basic_renderpasses[1<<3][BasicRenderPasses::eSizeBrp];
		vk::UniqueRenderPass                 m_crenderpass;


		std::unique_ptr<VulkanView> view_;


		//////////Frame render variables////////
		uint32_t					rv;
		uint32_t					imageIndex;
		vk::Result					rvRes;
		vk::Semaphore				waitSemaphores;
		vk::Semaphore				readySemaphores;
		vk::UniqueFence				imageFence;

		//This should be replaced with a signaling system of some sort
		bool						m_imguiNeedsToResize{ false };

		//vector<FrameSubmitRenderInfo> submitList;

		VulkanResourceManager rsc_manager;
		///////////////////////////////////////


		void createInstance();
		void createSurface(HINSTANCE winstance, HWND wnd);
		void pickPhysicalDevice();
		void createLogicalDevice();

		void createSwapChain();
		void createFrameObjects();
		void createImageViews();

		auto createShaderModule(const string_view& code) -> vk::UniqueShaderModule;

		void createRenderPass();

		
		//Texture functions start//
		vk::UniqueImageView createImageView(vk::UniqueImage& img, vk::Format format);
		//Texture functions end//

		void createCommandPool();
		void createCommandBuffers();
		void createSemaphores();

		void copyBuffer(vk::Buffer srcBuffer, vk::Buffer dstBuffer, vk::DeviceSize size);

		auto populateDebugMessengerCreateInfo(ValHandler* userData = nullptr) -> vk::DebugUtilsMessengerCreateInfoEXT;

		auto deviceSuitability(vk::PhysicalDevice const& pd) -> float;
		auto SelectDevice(std::vector<vk::PhysicalDevice> const& devices) -> vk::PhysicalDevice;

		std::vector<const char*>             GetValidationLayers();
		std::vector<const char*>             GetExtensions(vk::Optional<const std::string> = nullptr);
		std::vector<const char*>             GetDeviceExtensions();
		std::vector<vk::LayerProperties>     GetAllValidationLayers();
		std::vector<vk::ExtensionProperties> GetAllExtensions(vk::Optional<const std::string> layer = nullptr);
		template<typename T, typename F>
		bool CheckProperties (std::vector<T>const& properties, std::vector<const char*> const& desired, const F& func);
		vk::SurfaceFormatKHR                 chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats);
		vk::PresentModeKHR                   chooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes);

		vk::Extent2D                         chooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities);

		QueueFamilyIndices                   findQueueFamilies(vk::PhysicalDevice const& device);
		SwapChainSupportDetails              querySwapChainSupport(const vk::PhysicalDevice& device);
	};
}

#include <vkn/VulkanState.inl>