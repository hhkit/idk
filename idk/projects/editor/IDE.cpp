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

		//ImGui Initializations
		_interface->Init();
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags = ImGuiConfigFlags_DockingEnable;


		ige_main_window = std::make_unique<IGE_MainWindow>();

		ige_windows.push_back(std::make_unique<IGE_SceneView>());
		ige_windows.push_back(std::make_unique<IGE_ProjectWindow>());
		ige_windows.push_back(std::make_unique<IGE_HierarchyWindow>());
		ige_windows.push_back(std::make_unique<IGE_InspectorWindow>());

		ige_main_window->Initialize();

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

		ige_main_window->DrawWindow();

		for (auto& i : ige_windows) {
			i->DrawWindow();
		}

		if (bool_demo_window)
			ImGui::ShowDemoWindow(&bool_demo_window);
	
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
