#pragma once
#include <gfx/GraphicsSystem.h>
#define VK_USE_PLATFORM_WIN32_KHR
#ifdef free
#undef free
#endif
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_win32.h>
#include <vkn/GraphicsState.h>
#include<vkn/UboManager.h>
#include <vkn/FrameRenderer.h>

namespace idk::win
{
	class Windows;
}
namespace idk::vkn
{
	class PipelineManager;
	using Windows = win::Windows;
	class VulkanState;
	class VulkanDebugRenderer;

	class VulkanWin32GraphicsSystem
		: public GraphicsSystem
	{
	public:
		VulkanWin32GraphicsSystem();
		~VulkanWin32GraphicsSystem();
		void Init() override ;
		void LateInit() override;

		void BeginFrame() ;
		void EndFrame() ;
		void Shutdown() override;
		//void BufferGraphicsState(span<class MeshRenderer>, span<const class Transform>, span<const class Parent>) override {};
		GraphicsAPI GetAPI() override;
		void RenderRenderBuffer() override;
		void SwapBuffer() override;
		VulkanState& Instance() { return *instance_; }

		VulkanState& GetVulkanHandle();
	private:
		std::unique_ptr<VulkanState> instance_;
		vector<FrameRenderer> _frame_renderers;
		std::unique_ptr<PipelineManager> _pm;
		unique_ptr<VulkanDebugRenderer> _debug_renderer;
		win::Windows* windows_;
		template<typename T, typename D = vk::DispatchLoaderStatic>
		using VkHandle = vk::UniqueHandle<T, D>;
	};
}