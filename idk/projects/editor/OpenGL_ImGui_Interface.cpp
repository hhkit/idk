#include "pch.h"

#include <iostream>//std::cerr

#include "OpenGL_ImGui_Interface.h"

//#include <vulkan/VulkanWin32GraphicsSystem.h>
#include <win32/WindowsApplication.h>

//Imgui
#include <imgui/imgui.h>

//Opengl
#include <glad/glad.h>

//Helper header from imgui for vulkan implementation
#include <editorstatic/imgui/ImGui_impl_opengl3.h>
#include <editorstatic/imgui/ImGUIImpl_Win32.h>

namespace idk
{

	namespace edt {
		OI_Interface::OI_Interface(ogl::OpenGLState* o)
			:oglObj{ o }
		{}

		void OI_Interface::Init()
		{
			//Creation

			//IMGUI setup
			IMGUI_CHECKVERSION();

			ImGui::CreateContext();
			ImGuiIO& io = ImGui::GetIO();

			//Imgui Style
			ImGui::StyleColorsClassic();

			//Platform/renderer bindings
			ImGui_ImplWin32_Init(Core::GetSystem<Windows>().GetWindowHandle());

			Core::GetSystem<Windows>().PushWinProcEvent(ImGui_ImplWin32_WndProcHandler);

			ImGui_ImplOpenGL3_Init((const char*)"#version 420");

			editorControls.im_clearColor = vec4{ 0,0,0,1 };

			//Upload fonts leave it to later
			// Upload Fonts
		}

		void OI_Interface::Shutdown()
		{
			//editorInit.edt_pipeCache.reset();

			ImGui_ImplOpenGL3_Shutdown();
			ImGui_ImplWin32_Shutdown();
			ImGui::DestroyContext();
		}

		void OI_Interface::ImGuiFrameBegin()
		{
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();
		}

		void OI_Interface::ImGuiFrameUpdate()
		{

			ImGuiFrameBegin();

			//////////////////////////IMGUI DATA HANDLING//////////////////////
			if (editorControls.im_demoWindow)
				ImGui::ShowDemoWindow(&editorControls.im_demoWindow);

			static float f = 0.0f;
			static int counter = 0;


			ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

			ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
			ImGui::Checkbox("Demo Window", &editorControls.im_demoWindow);      // Edit bools storing our window open/close state

			ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
			ImGui::ColorEdit3("clear color", (float*)& editorControls.im_clearColor); // Edit 3 floats representing a color

			if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
				counter++;
			ImGui::SameLine();
			ImGui::Text("counter = %d", counter);

			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::End();
			///////////////////////////////END///////////////////////////////

			//ImGuiFrameEnd();
		}

		void OI_Interface::ImGuiFrameEnd()
		{
			ImGui::Render();
			//memcpy(&editorControls.edt_clearValue.color.float32[0], &editorControls.im_clearColor, 4 * sizeof(float));
			ImGuiFrameRender();
		}

		void OI_Interface::ImGuiFrameRender()
		{/*
			ImGuiIO& io = ImGui::GetIO();
			glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
			glClearColor(editorControls.im_clearColor.x, editorControls.im_clearColor.y, editorControls.im_clearColor.z, editorControls.im_clearColor.w);
			glClear(GL_COLOR_BUFFER_BIT);*/
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		}


	};
};