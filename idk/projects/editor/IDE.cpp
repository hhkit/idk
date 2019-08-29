#include "pch.h"
#include "IDE.h"

#include <imgui/imgui.h>
#include <vkn/VulkanWin32GraphicsSystem.h>
#include <idk_opengl/system/OpenGLGraphicsSystem.h>
#include <vkn/VulkanState.h>
#include <idk_opengl/system/OpenGLState.h>
#include <loading/OpenGLFBXLoader.h>

namespace idk
{
	IDE::IDE() :edtInterface_v{ nullptr }, edtInterface_o{ nullptr }
	{
	}

	void IDE::Init()
	{
		// do imgui stuff
		switch (Core::GetSystem<GraphicsSystem>().GetAPI())
		{
		case GraphicsAPI::OpenGL:
			edtInterface_o = edt::OI_Interface{ &Core::GetSystem<ogl::Win32GraphicsSystem>().Instance() };
			edtInterface_o.Init();
			Core::GetResourceManager().RegisterExtensionLoader<OpenGLFBXLoader>(".fbx");
			Core::GetResourceManager().RegisterExtensionLoader<OpenGLFBXLoader>(".md5mesh");
			break;
		case GraphicsAPI::Vulkan:
			edtInterface_v = edt::VI_Interface{ &Core::GetSystem<vkn::VulkanWin32GraphicsSystem>().Instance() };
			edtInterface_v.Init();		
			break;
		default:
			break;
		}
	}

	void IDE::Shutdown()
	{
		// close imgui stuff
		switch (Core::GetSystem<GraphicsSystem>().GetAPI())
		{
		case GraphicsAPI::Vulkan:
			edtInterface_v.Shutdown();
			break;
		case GraphicsAPI::OpenGL:
			edtInterface_o.Shutdown();
			break;
		};
	}

	void IDE::EditorUpdate()
	{
		// update editor vars
		switch (Core::GetSystem<GraphicsSystem>().GetAPI())
		{
		case GraphicsAPI::Vulkan:
			edtInterface_v.ImGuiFrameUpdate();
			break;
		case GraphicsAPI::OpenGL:
			edtInterface_o.ImGuiFrameUpdate();
			break;
		};
		//edtInterface.ImGuiFrameEnd();
	}

	void IDE::EditorDraw()
	{
		// call imgui draw,
		//edtInterface.ImGuiFrameEnd();
		switch (Core::GetSystem<GraphicsSystem>().GetAPI())
		{
		case GraphicsAPI::Vulkan:
			//edtInterface_v.ImGuiFrameUpdate();
			break;
		case GraphicsAPI::OpenGL:
			edtInterface_o.ImGuiFrameEnd();
			break;
		};
		Core::GetSystem<vkn::VulkanWin32GraphicsSystem>(); // get command buffer from vulkan
	}
}
