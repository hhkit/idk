#pragma once

namespace idk {
	namespace ogl {
		class OpenGLState;
	}
	namespace edt {
		
		class OI_Interface {
		public:
			OI_Interface(ogl::OpenGLState*);

			void Init();
			void Shutdown();

			void ImGuiFrameBegin();
			void ImGuiFrameUpdate();
			void ImGuiFrameEnd();

			void ImGuiFrameRender();
			//void ImGuiFramePresent();

			//void ImGuiRecreateSwapChain();
			//void ImGuiRecreateCommandBuffer();

			//void ImGuiCleanUpSwapChain();

		private:

			//struct EditorInitInfo
			//{
			//	//vk::UniquePipelineCache				edt_pipeCache{};
			//	//uint32_t							edt_min_imageCount{ 2 };
			//	//uint32_t							edt_imageCount{ edt_min_imageCount };
			//	//vk::SampleCountFlagBits				edt_sampleBits_MSAA{};
			//};
			struct EditorParameter
			{
				//ImGui
				bool								im_demoWindow{ true };
				vec4                                im_clearColor{};

				//Vulkan
				//vk::UniqueRenderPass				edt_renderPass{};
				bool								edt_clearEnable{true};
				//vk::ClearValue						edt_clearValue{};
				uint32_t							edt_frameIndex{};
				uint32_t							edt_imageCount{};
				uint32_t							edt_semaphoreIndex{};
				//vector<EditorFrame>					edt_frames{};
				//vector<EditorPresentationSignal>	edt_frameSemophores{};
			};

			EditorParameter				editorControls;
			//EditorInitInfo				editorInit;
			ogl::OpenGLState*			oglObj;
		};
	};
};