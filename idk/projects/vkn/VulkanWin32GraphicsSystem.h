#pragma once
#include <gfx/GraphicsSystem.h>
#define VK_USE_PLATFORM_WIN32_KHR
#ifdef free
#undef free
#endif
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_win32.h>
#include <vkn/GraphicsState.h>

namespace idk::win
{
	class Windows;
	
}
namespace idk::vkn
{
	namespace dbg
	{
		class time_log;
	}
	class FrameRenderer;
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

		void Shutdown() override;
		//void BufferGraphicsState(span<class MeshRenderer>, span<const class Transform>, span<const class Parent>) override {};
		GraphicsAPI GetAPI() override;
		void Prerender()override;
		void RenderRenderBuffer() override;
		void SwapBuffer() override;
		
		VulkanState& Instance() { return *instance_; }
		VulkanView&  View()const { return instance_->View();}

		dbg::time_log& TimeLog();

		VulkanState& GetVulkanHandle();
	private:
		void SkeletonToUniforms(const SkeletonTransforms& trf,hash_table<string,string>&)override;
		std::unique_ptr<VulkanState> instance_;
		void RenderBRDF(RscHandle<ShaderProgram> prog);

		vector<FrameRenderer> _frame_renderers;
		std::unique_ptr<PipelineManager> _pm;
		unique_ptr<VulkanDebugRenderer> _debug_renderer;
		win::Windows* windows_;
		template<typename T, typename D = vk::DispatchLoaderStatic>
		using VkHandle = vk::UniqueHandle<T, D>;

		struct Pimpl;
		std::unique_ptr<Pimpl> _pimpl;
	};
}