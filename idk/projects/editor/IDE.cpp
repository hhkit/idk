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
#include <loading/GraphFactory.h>
#include <loading/OpenGLCubeMapLoader.h>
#include <loading/OpenGLTextureLoader.h>
#include <editor/commands/CommandList.h>
#include <editor/windows/IGE_WindowList.h>
#include <res/EasyFactory.h>
#include <imgui/ImGuizmo.h>
#include <core/Scheduler.h>
#include <PauseConfigurations.h>
#include <editor/windows/IGE_ShadowMapWindow.h>

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
			break;
		case GraphicsAPI::Vulkan:
			_interface = std::make_unique<edt::VI_Interface>(&Core::GetSystem<vkn::VulkanWin32GraphicsSystem>().Instance());
			break;
		default:
			break;
		}
		Core::GetResourceManager().RegisterFactory<GraphFactory>();
        Core::GetSystem<Windows>().OnClosed.Listen([&]() { closing = true; });

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
        io.Fonts->AddFontFromFileTTF(fontpath.c_str(), 14.0f, &config);

		//Window Initializations
		ige_main_window = std::make_unique<IGE_MainWindow>();

#define ADD_WINDOW(type) windows_by_type.emplace(reflect::typehash<type>(), ige_windows.emplace_back(std::make_unique<type>()).get());
		ADD_WINDOW(IGE_SceneView);
		ADD_WINDOW(IGE_ShadowMapWindow);
		ADD_WINDOW(IGE_ProjectWindow);
		ADD_WINDOW(IGE_HierarchyWindow);
		ADD_WINDOW(IGE_InspectorWindow);
		ADD_WINDOW(IGE_MaterialEditor);
#undef ADD_WINDOW

		ige_main_window->Initialize();
		for (auto& i : ige_windows) {
			i->Initialize();
		}

	}

	void IDE::LateInit()
	{

		switch (Core::GetSystem<GraphicsSystem>().GetAPI())
		{
		case GraphicsAPI::OpenGL:
			Core::GetResourceManager().RegisterLoader<OpenGLFBXLoader>(".fbx");
			Core::GetResourceManager().RegisterLoader<OpenGLFBXLoader>(".obj");
			Core::GetResourceManager().RegisterLoader<OpenGLFBXLoader>(".md5mesh");
			Core::GetResourceManager().RegisterLoader<OpenGLCubeMapLoader>(".cbm");
			Core::GetResourceManager().RegisterLoader<OpenGLTextureLoader>(".png");
			Core::GetResourceManager().RegisterLoader<OpenGLTextureLoader>(".jpg");
			Core::GetResourceManager().RegisterLoader<OpenGLTextureLoader>(".jpeg");
			Core::GetResourceManager().RegisterLoader<OpenGLTextureLoader>(".dds");
			break;
		case GraphicsAPI::Vulkan:
			Core::GetResourceManager().RegisterLoader<VulkanFBXLoader>(".fbx");
			Core::GetResourceManager().RegisterLoader<VulkanFBXLoader>(".obj");
			Core::GetResourceManager().RegisterLoader<VulkanFBXLoader>(".md5mesh");
			break;
		default:
			break;
		}
		Core::GetResourceManager().RegisterLoader<GraphLoader>(shadergraph::Graph::ext);

		Core::GetScheduler().SetPauseState(EditorPause);
	}

	void IDE::Shutdown()
	{
        ige_windows.clear();
		_interface->Shutdown();
		_interface.reset();
	}

	void IDE::EditorUpdate()
	{
        if (closing)
            return;

		_interface->ImGuiFrameBegin();
		ImGuizmo::BeginFrame();



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

	void IDE::FocusOnSelectedGameObjects()
	{
		if (selected_gameObjects.size()) {
			vec3 finalCamPos{};
			for (Handle<GameObject> i : selected_gameObjects) {

				Handle<Transform> transform = i->GetComponent<Transform>();
				if (transform) {
					finalCamPos += transform->position;
				}

			}

            finalCamPos /= static_cast<float>(selected_gameObjects.size());

			const float distanceFromObject = 10; //Needs to be dependent of spacing of objects

			CameraControls& main_camera = Core::GetSystem<IDE>()._interface->Inputs()->main_camera;
			Handle<Camera> currCamera = main_camera.current_camera;
			Handle<Transform> camTransform = currCamera->GetGameObject()->GetComponent<Transform>();
			camTransform->position = finalCamPos;
			focused_vector = finalCamPos;
			scroll_multiplier = default_scroll_multiplier;
			camTransform->position += camTransform->Forward() * distanceFromObject;
		}
	}

}
