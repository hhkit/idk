#include "pch.h"
#include "IDE.h"

#include <imgui/imgui.h>
#include <vkn/VulkanWin32GraphicsSystem.h>
#include <idk_opengl/system/OpenGLGraphicsSystem.h>
#include <editor/Vulkan_ImGui_Interface.h>
#include <editor/OpenGL_ImGui_Interface.h>
#include <vkn/VulkanState.h>
#include <idk_opengl/system/OpenGLState.h>
#include <loading/OpenGLFBXLoader.h>
#include <editor/windows/IGE_WindowList.h>
#include <editor/commands/CommandList.h>

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
			break;
		default:
			break;
		}

		_interface->Init();
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags = ImGuiConfigFlags_DockingEnable;
		ige_windows.push_back(std::make_unique<IGE_MainWindow>());


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

		_interface->ImGuiFrameUpdate();
		
		
		
		
		
		_interface->ImGuiFrameEnd();
	}

	void IDE::EditorDraw()
	{
		// call imgui draw,
		_interface->ImGuiFrameRender();
	}
}
