#pragma once

#include <vulkan/vulkan.hpp>

namespace idk {
	namespace vkn {
		class VulkanState;
	}
	namespace edt {
		struct EditorFrame
		{
			vk::UniqueCommandPool				edt_cPool{};
			vk::UniqueCommandBuffer				edt_cBuffer{};
			vk::UniqueFence						edt_fence{};
			vk::UniqueImage						edt_backbuffer{};
			vk::ImageView					    edt_backbufferView{};
			vk::UniqueFramebuffer				edt_framebuffer{};
		};
		struct EditorPresentationSignal
		{
			vk::UniqueSemaphore					edt_imageAvailable{};
			vk::UniqueSemaphore					edt_renderFinished{};
			vk::UniqueFence						edt_inflight_fence{};
		};
		class VI_Interface {
		public:
			VI_Interface(vkn::VulkanState*);

			void Init();
			void Shutdown();

			void ImGuiFrameBegin();
			void ImGuiFrameUpdate();
			void ImGuiFrameEnd();

			void ImGuiFrameRender();
			void ImGuiFramePresent();

			void ImGuiRecreateSwapChain();
			void ImGuiRecreateCommandBuffer();

		private:

			struct EditorInitInfo
			{
				vk::UniquePipelineCache				edt_pipeCache{};
				uint32_t							edt_min_imageCount{ 2 };
				uint32_t							edt_imageCount{ edt_min_imageCount };
				vk::SampleCountFlagBits				edt_sampleBits_MSAA{};
			};
			struct EditorParameter
			{
				//ImGui
				bool								im_demoWindow{ true };
				vec4                                im_clearColor{};

				//Vulkan
				vk::UniqueRenderPass				edt_renderPass{};
				bool								edt_clearEnable{};
				vk::ClearValue						edt_clearValue{};
				uint32_t							edt_frameIndex{};
				uint32_t							edt_imageCount{};
				uint32_t							edt_semaphoreIndex{};
				vector<EditorFrame>					edt_frames{};
				vector<EditorPresentationSignal>	edt_frameSemophores{};
			};

			EditorParameter				editorControls;
			EditorInitInfo				editorInit;
			vkn::VulkanState*			vkObj;
		};
	};
};