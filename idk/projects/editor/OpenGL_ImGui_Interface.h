#pragma once
#include "ImGui_Interface.h"
#include "EditorInputs.h"

namespace idk {
	namespace ogl {
		class OpenGLState;
	}
	namespace edt {
		
		class OI_Interface :public I_Interface {
		public:
			OI_Interface(ogl::OpenGLState*);

			void Init() override;
			void Shutdown() override;

			void ImGuiFrameBegin() override;
			void ImGuiFrameUpdate() override;
			void ImGuiFrameEnd() override;

			void ImGuiFrameRender() override;

			EditorInputs* Inputs() override;
			
			void TestFunction();

		private:

			
			struct EditorParameter
			{
				//ImGui
				bool					im_demoWindow{ true };
				vec4                    im_clearColor{};

				//OpenGL
				
			};

			EditorParameter				editorControls;
			ogl::OpenGLState*			oglObj;
			EditorInputs				editorInputs;

		};
	};
};