#pragma once
#include <gfx/GraphicsSystem.h>
#define VK_USE_PLATFORM_WIN32_KHR
#ifdef free
#undef free
#endif
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_win32.h>
#include<vkn/UboManager.h>

namespace idk::win
{
	class Windows;
}
namespace idk::vkn
{
	struct RenderStateV2
	{
		vk::CommandBuffer cmd_buffer;
		UboManager ubo_manager;
		bool has_commands = false;
		void FlagRendered() { has_commands = true; }
		void Reset();
		RenderStateV2() = default;
		RenderStateV2(const RenderStateV2&) = delete;
		RenderStateV2(RenderStateV2&&) = default;
	};

	struct RenderFrameObject
	{
		vector<RenderStateV2> states;
		vk::UniqueCommandBuffer pri_buffer;
		vk::UniqueCommandBuffer transition_buffer;
	};
	using Windows = win::Windows;
	class VulkanState;

	class VulkanWin32GraphicsSystem
		: public GraphicsSystem
	{
	public:
		VulkanWin32GraphicsSystem();
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
		void RenderGraphicsState(const GraphicsState&,RenderStateV2& render_state);
		std::unique_ptr<VulkanState> instance_;
		vector<vk::UniqueCommandBuffer> cmd_buffers;
		vector<RenderFrameObject> frames;
		win::Windows* windows_;
		template<typename T, typename D = vk::DispatchLoaderStatic>
		using VkHandle = vk::UniqueHandle<T, D>;
	};
}