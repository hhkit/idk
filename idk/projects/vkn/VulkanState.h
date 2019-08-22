#include "pch.h"
#pragma once
#include <optional>
#include <fstream>
#include <sstream>
#include "math2.h"
#pragma warning(disable:26495)
#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_win32.h>
#undef max
#undef min

namespace idk::vkn 
{
	struct window_info
	{
		ivec2     size;
		HWND      wnd;
		HINSTANCE winstance;
	};

	class VulkanState;

	struct QueueFamilyIndices
	{
		std::optional<uint32_t>      graphics_family;
		std::optional<uint32_t>      present_family;
		std::optional<uint32_t>      transfer_family;
		std::unordered_set<uint32_t> unique_queues()const;
		bool isComplete()const;
	};
	struct SwapChainSupportDetails {
		vk::SurfaceCapabilitiesKHR capabilities;
		std::vector<vk::SurfaceFormatKHR> formats;
		std::vector<vk::PresentModeKHR> presentModes;
	};
	struct SwapChainInfo
	{
		uint32_t curr_index{};
		vk::UniqueSwapchainKHR           swap_chain;
		vk::Format format;
		vk::Extent2D extent;
		std::vector<vk::Image            > images;
		std::vector<vk::UniqueImageView  > image_views;
		std::vector<vk::UniqueFramebuffer> frame_buffers;

		std::vector<std::pair<vk::UniqueBuffer, vk::UniqueDeviceMemory>> uniform_buffers;
		std::vector<std::pair<vk::UniqueBuffer, vk::UniqueDeviceMemory>> uniform_buffers2;
		std::vector<vk::DescriptorSet    > descriptor_sets;
		std::vector<vk::DescriptorSet    > descriptor_sets2;

		struct UniformStuff
		{
			std::pair<vk::UniqueBuffer, vk::UniqueDeviceMemory> uniform_buffer;
			vk::DescriptorSet descriptor_set;

		};
		struct Uniforms
		{
			vk::UniqueDescriptorSetLayout        layout;

			vector<UniformStuff> frame;
			void Init(size_t num) { frame.resize(num); }
			auto& uniform_buffer(size_t index) { return frame[index].uniform_buffer; }
			auto& descriptor_set(size_t index) { return frame[index].descriptor_set; }

			decltype(frame)::iterator begin() { return frame.begin(); }
			decltype(frame)::iterator end  () { return frame.end  (); }
			uint32_t size() { return s_cast<uint32_t>(frame.size()); }
		};
		//struct UniformManager
		//{
		//	struct BindingInfo
		//	{
		//		uint32_t offset;
		//		uint32_t size;
		//	};
		//	hlp::vector_buffer master_bo;
		//	string             local_buffer;
		//	vector<BindingInfo> bindings;
		//};
		Uniforms uniforms2;
	};
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

		struct ValHandler
		{
			virtual VkBool32 processMsg(
				[[maybe_unused]] VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
				[[maybe_unused]] VkDebugUtilsMessageTypeFlagsEXT messageType,
				[[maybe_unused]] const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData);
		};

		VulkanState();
		~VulkanState();

		void UpdateWindowSize(vec2 size);
		void CleanupSwapChain();
		void RecreateSwapChain();

		void InitVulkanEnvironment(window_info info);
		void NextFrame();


#pragma region ("Potentially main thing")
		VulkanView& View();
		unique_vbo      CreateVbo(void const* buffer_start, void const* buffer_end);
		unique_ubo      CreateUbo(void const* buffer_start, void const* buffer_end);
		unique_pipeline CreatePipeline(pipeline_config const& config);
		void Draw(unique_vbo const& vbo, unique_ubo const& uniforms, unique_pipeline const& pipeline);
#pragma endregion

		void BeginFrame();
		void EndFrame();

		void DrawFrame();
		void OnResize();
		void Cleanup();

		static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
			VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
			VkDebugUtilsMessageTypeFlagsEXT messageType,
			const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
			void* pUserData);

	private:
		// type aliases
		friend class VulkanView;
		template<typename T>
		using DynamicHandle = vk::UniqueHandle<T, vk::DispatchLoaderDynamic>;
		struct PresentationSignals
		{
			vk::UniqueSemaphore image_available;
			vk::UniqueSemaphore render_finished;
			vk::UniqueFence     inflight_fence;
		};

		// variables


		// init info
		vk::UniqueInstance               instance;
		vk::PhysicalDevice               pdevice = {};
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
		uint32_t WIDTH = 1280, HEIGHT = 720;
		uint32_t current_frame = 0, max_frames_in_flight = 2;
		vk::UniqueSurfaceKHR                 m_surface;



		vk::UniqueCommandPool                m_commandpool;
		std::vector<vk::UniqueCommandBuffer> m_commandbuffers;
		std::vector<vk::UniqueCommandBuffer> m_commandbuffers2;
		std::vector<vk::UniqueCommandBuffer> m_pri_commandbuffers;
		std::vector<PresentationSignals>     m_pres_signals;
		SwapChainInfo                        m_swapchain;

		vk::UniqueRenderPass                 m_renderpass;
		vk::UniqueDescriptorSetLayout        m_descriptorsetlayout;
		vk::UniquePipelineLayout             m_pipelinelayout;
		vk::UniquePipeline                   m_pipeline;

		std::vector<vk::UniqueDeviceMemory>  m_vertex_memories;
		std::vector<vk::UniqueBuffer      >  m_vertex_buffers;
		vk::UniqueDeviceMemory               m_ib_memory;
		vk::UniqueBuffer                     m_index_buffer;

		std::unique_ptr<VulkanView> view_;


		void createInstance();
		void createSurface(HINSTANCE winstance, HWND wnd);
		void pickPhysicalDevice();
		void createLogicalDevice();

		void createSwapChain();

		void createImageViews();

		auto createShaderModule(const std::string& code) -> vk::UniqueShaderModule;

		void createRenderPass();

		void createDescriptorSetLayout();
		void createGraphicsPipeline();

		void createFramebuffers();

		void createCommandPool();
		void createVertexBuffers();
		void createIndexBuffers();
		void createUniformBuffers();
		void createDescriptorPool();
		void createDescriptorSet();
		void createCommandBuffers();
		void createSemaphores();

		void copyBuffer(vk::Buffer srcBuffer, vk::Buffer dstBuffer, vk::DeviceSize size);
		void updateUniformBuffer(uint32_t image_index);

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