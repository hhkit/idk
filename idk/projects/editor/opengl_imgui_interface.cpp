#include "pch.h"
#include "opengl_imgui_interface.h"

#include <win32/WindowsApplication.h>

//Imgui
#include <imgui/imgui.h>
#include <imgui/imgui_impl_opengl3.h>
#include <imgui/imgui_impl_win32.h>

//Opengl
#include <glad/glad.h>

#include <iostream> //std::cerr

namespace idk
{
	opengl_imgui_interface::opengl_imgui_interface(ogl::OpenGLState* o)
		:oglObj{ o }
	{}

	void opengl_imgui_interface::Init()
	{
		//Creation

		//IMGUI setup
		IMGUI_CHECKVERSION();

		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io;

		//Platform/renderer bindings
		ImGui_ImplWin32_Init(Core::GetSystem<Windows>().GetWindowHandle());

		Core::GetSystem<Windows>().PushWinProcEvent(ImGui_ImplWin32_WndProcHandler);

		ImGui_ImplOpenGL3_Init((const char*)"#version 420");

		editorControls.im_clearColor = vec4{ 0,0,0,1 };

		//Upload fonts leave it to later
		// Upload Fonts
	}

	void opengl_imgui_interface::Shutdown()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
	}

	void opengl_imgui_interface::ImGuiFrameBegin()
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
	}

	void opengl_imgui_interface::ImGuiFrameUpdate()
	{

		//ImGuiFrameBegin();

		//////////////////////////IMGUI DATA HANDLING//////////////////////
		if (editorControls.im_demoWindow)
			ImGui::ShowDemoWindow(&editorControls.im_demoWindow);

		static float f = 0.0f;
		static int counter = 0;


		ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

		ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
		ImGui::Checkbox("Demo Window", &editorControls.im_demoWindow);      // Edit bools storing our window open/close state

		ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
		ImGui::ColorEdit3("clear color", (float*)&editorControls.im_clearColor); // Edit 3 floats representing a color

		if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
			counter++;
		ImGui::SameLine();
		ImGui::Text("counter = %d", counter);

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();
		///////////////////////////////END///////////////////////////////

		//ImGuiFrameEnd();
	}

	void opengl_imgui_interface::ImGuiFrameEnd()
	{
		ImGui::Render();
	}

	void opengl_imgui_interface::ImGuiFrameRender()
	{
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	void opengl_imgui_interface::TestFunction()
	{
		static float f = 0.0f;
		static int counter = 0;


		ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

		ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
		ImGui::Checkbox("Demo Window", &editorControls.im_demoWindow);      // Edit bools storing our window open/close state

		ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
		ImGui::ColorEdit3("clear color", (float*)&editorControls.im_clearColor); // Edit 3 floats representing a color

		if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
			counter++;
		ImGui::SameLine();
		ImGui::Text("counter = %d", counter);

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();
	}

}