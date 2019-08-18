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
#include <utils/Utils.h>
#undef max
#undef min

struct window_info
{
	idk::ivec2 size;
	HWND wnd;
	HINSTANCE winstance;
};

class Vulkan;

struct debug_info;
namespace vgfx
{
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
		vk::UniqueSwapchainKHR           swap_chain;
		vk::Format format;
		vk::Extent2D extent;
		std::vector<vk::Image            > images;
		std::vector<vk::UniqueImageView  > image_views;
		std::vector<vk::UniqueFramebuffer> frame_buffers;

		std::vector<std::pair<vk::UniqueBuffer, vk::UniqueDeviceMemory>> uniform_buffers;
		std::vector<vk::DescriptorSet    > descriptor_sets;
	};
	class VulkanDetail;
}

class Vulkan
{
public:
	using QueueFamilyIndices     =vgfx::QueueFamilyIndices;
	using SwapChainInfo          =vgfx::SwapChainInfo;
	using SwapChainSupportDetails=vgfx::SwapChainSupportDetails;
	struct vbo     ;//vertex buffer object
	struct ubo     ;//uniform buffer object
	struct pipeline;

	struct pipeline_config;

	using unique_vbo      = std::unique_ptr<vbo     >;
	using unique_ubo      = std::unique_ptr<ubo     >;
	using unique_pipeline = std::unique_ptr<pipeline>;

	struct ValHandler;
	void UpdateWindowSize(vec2 size);
	struct ValHandler
	{
		virtual VkBool32 processMsg(
			[[maybe_unused]] VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
			[[maybe_unused]] VkDebugUtilsMessageTypeFlagsEXT messageType,
			[[maybe_unused]] const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData);
	};
	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData);
	void CleanupSwapChain();
	void RecreateSwapChain();

	void InitVulkanEnvironment(window_info info);// [[maybe_unused]] WindowsProgram& wp, [[maybe_unused]] std::shared_ptr<WindowsWindow> window);
	void NextFrame();


#pragma region ("Potentially main thing")
	vgfx::VulkanDetail& GetDetail();
	unique_vbo      CreateVbo     (void const* buffer_start, void const* buffer_end);
	unique_ubo      CreateUbo     (void const* buffer_start, void const* buffer_end);
	unique_pipeline CreatePipeline(pipeline_config const& config);
	void Draw(unique_vbo const& vbo, unique_ubo const& uniforms, unique_pipeline const& pipeline);
#pragma endregion
	void BeginFrame();
	void EndFrame();

	void DrawFrame();
	void OnResize();
	void Cleanup();
	Vulkan();
	~Vulkan();
private:
	void createInstance();
	void createSurface(HINSTANCE winstance, HWND wnd);
	void pickPhysicalDevice();
	void createLogicalDevice();

	void createSwapChain();

	void createImageViews();

	vk::UniqueShaderModule createShaderModule(const std::string& code);

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

	//Temporary, should move all the data/states into VulkanDetail
	friend vgfx::VulkanDetail;

	vk::DebugUtilsMessengerCreateInfoEXT populateDebugMessengerCreateInfo(ValHandler* userData = nullptr);

	window_info m_window;
	bool m_ScreenResized = false;
	uint32_t WIDTH = 1280, HEIGHT = 720;
	uint32_t current_frame = 0, max_frames_in_flight = 2;

	std::unique_ptr<vgfx::VulkanDetail> detail_;

	vk::DispatchLoaderDefault        dispatcher = {};
	vk::UniqueInstance               instance;
	vk::DispatchLoaderDynamic        dyn_dispatcher = {};
	vk::UniqueHandle<vk::DebugUtilsMessengerEXT, vk::DispatchLoaderDynamic>/*
	vk::DebugUtilsMessengerEXT//*/
		m_debug_messenger;
	vk::UniqueSurfaceKHR             m_surface;
	vk::PhysicalDevice               pdevice = {};
	vk::UniqueDevice                 m_device;
	QueueFamilyIndices m_queue_family = {};
	vk::Queue          m_graphics_queue = {};
	vk::Queue          m_present_queue  = {};
	//vk::Queue          m_transfer_queue = {};
	SwapChainInfo                    m_swapchain;

	vk::UniqueRenderPass                 m_renderpass;
	vk::UniqueDescriptorSetLayout        m_descriptorsetlayout;
	vk::UniquePipelineLayout             m_pipelinelayout;
	vk::UniquePipeline                   m_pipeline;
	vk::UniqueCommandPool                m_commandpool;
	vk::UniqueDescriptorPool             m_descriptorpool;

	//vk::UniqueDeviceMemory               m_device_memory;
	//vk::UniqueBuffer                     m_vertex_buffer;
	std::vector<vk::UniqueDeviceMemory>  m_vertex_memories;
	std::vector<vk::UniqueBuffer      >  m_vertex_buffers;
	vk::UniqueDeviceMemory               m_ib_memory;
	vk::UniqueBuffer                     m_index_buffer;

	std::vector<vk::UniqueCommandBuffer> m_commandbuffers;

	struct PresentationSignals
	{
		vk::UniqueSemaphore image_available;
		vk::UniqueSemaphore render_finished;
		vk::UniqueFence     inflight_fence;
	};

	std::vector<PresentationSignals> m_pres_signals;

	float deviceSuitability(vk::PhysicalDevice const& pd);
	vk::PhysicalDevice SelectDevice(std::vector<vk::PhysicalDevice> const& devices);


	std::vector<const char*>             GetValidationLayers();
	std::vector<const char*> GetExtensions(vk::Optional<const std::string> = nullptr);
	std::vector<const char*> GetDeviceExtensions();
	std::vector<vk::LayerProperties>     GetAllValidationLayers();
	std::vector<vk::ExtensionProperties> GetAllExtensions(vk::Optional<const std::string> layer = nullptr);
	template<typename T, typename F>
	bool CheckProperties(std::vector<T>const& properties, std::vector<const char*> const& desired, const F& func);
	vk::SurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats);
	vk::PresentModeKHR chooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes);

	vk::Extent2D chooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities);

	QueueFamilyIndices findQueueFamilies(vk::PhysicalDevice const& device);
	SwapChainSupportDetails querySwapChainSupport(const vk::PhysicalDevice& device);
};

#include "Vulkan.inl"