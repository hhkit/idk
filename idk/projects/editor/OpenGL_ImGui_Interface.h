#pragma once

#include "imgui_interface.h"

namespace idk
{
	namespace ogl {
		class OpenGLState;
	}

	class opengl_imgui_interface : public imgui_interface
	{
	public:
		opengl_imgui_interface(ogl::OpenGLState*);

		void Init() override;
		void Shutdown() override;

		void ImGuiFrameBegin() override;
		void ImGuiFrameUpdate() override;
		void ImGuiFrameEnd() override;

		void ImGuiFrameRender() override;

		void TestFunction();

	private:
		struct EditorParameter
		{
			//ImGui
			bool					im_demoWindow{ true };
			vec4                    im_clearColor{};
		};

		EditorParameter				editorControls;
		ogl::OpenGLState*			oglObj;
	};
}