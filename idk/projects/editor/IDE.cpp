//////////////////////////////////////////////////////////////////////////////////
//@file		IDE.cpp
//@author	Muhammad Izha B Rahim, Wei Xiang
//@param	Email : izha95\@hotmail.com
//@date		9 SEPT 2019
//@brief	

/*
Main Brain of the Game Editor

Accessible through Core::GetSystem<IDE>() [#include <IDE.h>]

*/
//////////////////////////////////////////////////////////////////////////////////


#include "pch.h"
#include <IDE.h>

#include <imgui/imgui.h>
#include <vkn/VulkanWin32GraphicsSystem.h>
#include <idk_opengl/system/OpenGLGraphicsSystem.h>
#include <editor/Vulkan_ImGui_Interface.h>
#include <editor/OpenGL_ImGui_Interface.h>
#include <win32/WindowsApplication.h>
#include <vkn/VulkanState.h>
#include <idk_opengl/system/OpenGLState.h>
#include <loading/OpenGLFBXLoader.h>
#include <loading/VulkanFBXLoader.h>
#include <loading/OpenGLCubeMapLoader.h>
#include <loading/OpenGLTextureLoader.h>
#include <editor/commands/CommandList.h>
#include <editor/windows/IGE_WindowList.h>
#include <gfx/ShaderGraphFactory.h>
#include <res/ForwardingExtensionLoader.h>


namespace idk
{
	IDE::IDE()
	{
	}

	void IDE::Init()
	{
		// do imgui stuff
		switch (Core::GetSystem<GraphicsSystem>().GetAPI())
		{
		case GraphicsAPI::OpenGL:
			_interface = std::make_unique<edt::OI_Interface>(&Core::GetSystem<ogl::Win32GraphicsSystem>().Instance());
			Core::GetResourceManager().RegisterExtensionLoader<OpenGLFBXLoader>(".fbx");
			Core::GetResourceManager().RegisterExtensionLoader<OpenGLFBXLoader>(".obj");
			Core::GetResourceManager().RegisterExtensionLoader<OpenGLFBXLoader>(".md5mesh");
			Core::GetResourceManager().RegisterExtensionLoader<OpenGLCubeMapLoader>(".cbm");
			break;
		case GraphicsAPI::Vulkan:
			_interface = std::make_unique<edt::VI_Interface>(&Core::GetSystem<vkn::VulkanWin32GraphicsSystem>().Instance());
			Core::GetResourceManager().RegisterExtensionLoader<VulkanFBXLoader>(".fbx");
			Core::GetResourceManager().RegisterExtensionLoader<VulkanFBXLoader>(".obj");
			Core::GetResourceManager().RegisterExtensionLoader<VulkanFBXLoader>(".md5mesh");
			break;
		default:
			break;
		}

        Core::GetSystem<Windows>().OnClosed.Listen([&]() { closing = true; });

        Core::GetResourceManager().RegisterFactory<shadergraph::Factory>();
        Core::GetResourceManager().RegisterExtensionLoader<ForwardingExtensionLoader<shadergraph::Graph>>(".mat");

        auto& fs = Core::GetSystem<FileSystem>();
        fs.Mount(string{ fs.GetExeDir() } + "/editor_data", "/editor_data", false);

		//ImGui Initializations
		_interface->Init();
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags = ImGuiConfigFlags_DockingEnable;

        //Imgui Style
        auto& style = ImGui::GetStyle();
        style.FramePadding = ImVec2(4.0f, 1.0f);
        style.WindowRounding = 0;
        style.TabRounding = 0;
        style.IndentSpacing = 14.0f;
        style.ScrollbarRounding = 0;
        style.GrabRounding = 0;
        style.ChildRounding = 0;
        style.PopupRounding = 0;
        style.CurveTessellationTol = 0.5f;

        auto* colors = style.Colors;
        ImGui::StyleColorsDark();

        colors[ImGuiCol_CheckMark] = colors[ImGuiCol_Text];

        // grays
        colors[ImGuiCol_WindowBg] =
            ImColor(29, 34, 41).Value;
        colors[ImGuiCol_PopupBg] =
        colors[ImGuiCol_ScrollbarBg] =
            ImColor(43, 49, 56, 240).Value;
        colors[ImGuiCol_Border] =
        colors[ImGuiCol_Separator] =
        colors[ImGuiCol_ScrollbarGrab] =
            ImColor(63, 70, 77).Value;
            //ImColor(106, 118, 129).Value;

        colors[ImGuiCol_MenuBarBg] =
            ImColor(36, 58, 74).Value;
        colors[ImGuiCol_ScrollbarBg].w = 0.5f;

        // main accent - 2
        colors[ImGuiCol_TitleBg] =
            ImColor(5, 30, 51).Value;

        // main accent - 1
        colors[ImGuiCol_TitleBgActive] =
        colors[ImGuiCol_TabUnfocused] =
            ImColor(11, 54, 79).Value;

        // main accent
        colors[ImGuiCol_Tab] =
        colors[ImGuiCol_TabUnfocusedActive] =
        colors[ImGuiCol_FrameBg] =
        colors[ImGuiCol_Button] =
        colors[ImGuiCol_Header] =
        colors[ImGuiCol_SeparatorHovered] =
        colors[ImGuiCol_ScrollbarGrabHovered] =
            ImColor(23, 75, 111).Value;

        // main accent + 1
        colors[ImGuiCol_TabHovered] =
        colors[ImGuiCol_TabActive] =
        colors[ImGuiCol_ButtonHovered] =
        colors[ImGuiCol_FrameBgHovered] =
        colors[ImGuiCol_HeaderHovered] =
        colors[ImGuiCol_SeparatorActive] =
        colors[ImGuiCol_ScrollbarGrabActive] =
            ImColor(46, 115, 143).Value;

        // main accent + 2
        colors[ImGuiCol_TextSelectedBg] =
        colors[ImGuiCol_ButtonActive] =
        colors[ImGuiCol_FrameBgActive] =
        colors[ImGuiCol_HeaderActive] =
            ImColor(65, 153, 163).Value;

        // font config
        ImFontConfig config;
        config.OversampleH = 5;
        config.OversampleV = 3;
        config.RasterizerMultiply = 1.5f;
        auto fontpath = fs.GetFullPath("/editor_data/fonts/SourceSansPro-Regular.ttf");
        io.Fonts->AddFontFromFileTTF(fontpath.c_str(), 16.0f, &config);

		//Window Initializations
		ige_main_window = std::make_unique<IGE_MainWindow>();

		ige_windows.push_back(std::make_unique<IGE_SceneView>());
		ige_windows.push_back(std::make_unique<IGE_ProjectWindow>());
		ige_windows.push_back(std::make_unique<IGE_HierarchyWindow>());
		ige_windows.push_back(std::make_unique<IGE_InspectorWindow>());
		ige_windows.push_back(std::make_unique<IGE_MaterialEditor>());

		ige_main_window->Initialize();

		//ige_main_window->Initialize();
		for (auto& i : ige_windows) {
			i->Initialize();
		}

	}

	void IDE::Shutdown()
	{
		_interface->Shutdown();
		_interface.reset();
	}

	void IDE::EditorUpdate()
	{
        if (closing)
            return;

		_interface->ImGuiFrameBegin();

		ige_main_window->DrawWindow();

		for (auto& i : ige_windows) {
			i->DrawWindow();
		}

		if (bool_demo_window)
			ImGui::ShowDemoWindow(&bool_demo_window);


		//_interface->Inputs()->Update(); //Moved to SceneView.cpp
		


		//_interface->ImGuiFrameUpdate();
		
		
		_interface->ImGuiFrameEnd();
	}

	void IDE::EditorDraw()
	{
		// call imgui draw,
		_interface->ImGuiFrameRender();
	}
	CameraControls& IDE::currentCamera()
	{
		// TODO: insert return statement here
		return _interface->Inputs()->main_camera;
	}
}
