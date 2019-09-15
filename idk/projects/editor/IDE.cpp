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
#include <vkn/VulkanState.h>
#include <idk_opengl/system/OpenGLState.h>
#include <loading/OpenGLFBXLoader.h>
#include <loading/VulkanFBXLoader.h>
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
			Core::GetResourceManager().RegisterExtensionLoader<OpenGLFBXLoader>(".md5mesh");
			break;
		case GraphicsAPI::Vulkan:
			_interface = std::make_unique<edt::VI_Interface>(&Core::GetSystem<vkn::VulkanWin32GraphicsSystem>().Instance());
			Core::GetResourceManager().RegisterExtensionLoader<VulkanFBXLoader>(".fbx");
			Core::GetResourceManager().RegisterExtensionLoader<VulkanFBXLoader>(".md5mesh");
			break;
		default:
			break;
		}

        Core::GetResourceManager().RegisterFactory<shadergraph::Factory>();
        Core::GetResourceManager().RegisterExtensionLoader<ForwardingExtensionLoader<shadergraph::Graph>>(".mat");

        auto& fs = Core::GetSystem<FileSystem>();
        fs.Mount(string{ fs.GetExeDir() } + "/editor_data", "/editor_data", false);

		//ImGui Initializations
		_interface->Init();
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags = ImGuiConfigFlags_DockingEnable;

        //Imgui Style
        ImGui::StyleColorsClassic();

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
		_interface->ImGuiFrameBegin();

		for (auto& i : ige_windows) {
			i->DrawWindow();
		}

		_interface->Inputs()->Update();
		
		_interface->ImGuiFrameUpdate();
		
		
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
