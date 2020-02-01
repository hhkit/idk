#pragma once

#include <imgui_interface.h>

#include <vulkan/vulkan.hpp>
#include <vkn/utils/TriBuffer.h>

namespace idk
{
	namespace vkn {
		class VulkanState;
		struct TriBuffer;
	}

	struct EditorFrame
	{
		vk::UniqueCommandPool				edt_cPool{};
		vk::UniqueCommandBuffer				edt_cBuffer{};
		vk::UniqueFence						edt_fence{};
		vk::Image						    edt_backbuffer{};
		vk::ImageView					    edt_backbufferView{};
		vk::UniqueFramebuffer				edt_framebuffer{};
	};
	struct EditorPresentationSignal
	{
		vk::UniqueSemaphore					edt_imageAvailable{};
		vk::UniqueSemaphore					edt_renderFinished{};
		vk::UniqueFence						edt_inflight_fence{};
	};

	class vulkan_imgui_interface : public imgui_interface
	{
	public:
		vulkan_imgui_interface(vkn::VulkanState*);

		void Init() override;
		void Shutdown() override;

		void ImGuiFrameBegin() override;
		void ImGuiFrameUpdate() override;
		void ImGuiFrameEnd() override;

		void ImGuiFrameRender() override;
		void ImGuiFramePresent();

		void ImGuiRecreateSwapChain();
		void ImGuiRecreateCommandBuffer();
		void ImGuiResizeWindow();

		void ImGuiCleanUpSwapChain();

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
			bool								edt_clearEnable{ true };
			vk::ClearValue						edt_clearValue{};
			uint32_t							edt_frameIndex{};
			uint32_t							edt_imageCount{};
			uint32_t							edt_semaphoreIndex{};
			vector<EditorFrame>					edt_frames{};
			vector<EditorPresentationSignal>	edt_frameSemophores{};

			shared_ptr<vkn::TriBuffer>          edt_buffer{};
			uint32_t							edt_submitInfoIndex{ 0 };
		};

		bool						font_initialized = false;
		EditorParameter				editorControls;
		EditorInitInfo				editorInit;
		vkn::VulkanState*			vkObj;
	};
}