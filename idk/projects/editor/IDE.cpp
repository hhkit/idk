#include "pch.h"
#include "IDE.h"

#include <imgui/imgui.h>
#include <vkn/VulkanWin32GraphicsSystem.h>
#include <idk_opengl/system/OpenGLGraphicsSystem.h>
#include <vkn/VulkanState.h>
#include <idk_opengl/system/OpenGLState.h>

namespace idk
{
	IDE::IDE(GraphicsAPI whichone) :edtInterface_v{ nullptr }, edtInterface_o{ nullptr }, gLibVer{whichone}
	{
	}

	void IDE::Init()
	{
		// do imgui stuff

		switch (gLibVer)
		{
		case GraphicsAPI::Vulkan:

			edtInterface_v = edt::VI_Interface{ &Core::GetSystem<vkn::VulkanWin32GraphicsSystem>().Instance() };
			edtInterface_v.Init();
			break;
		case GraphicsAPI::OpenGL:

			edtInterface_o = edt::OI_Interface{ &Core::GetSystem<ogl::Win32GraphicsSystem>().Instance() };
			edtInterface_o.Init();
			break;
		};
	}

	void IDE::Shutdown()
	{
		// close imgui stuff
		switch (gLibVer)
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
		switch (gLibVer)
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
		switch (gLibVer)
		{
		case GraphicsAPI::Vulkan:
			//edtInterface_v.ImGuiFrameUpdate();
			break;
		case GraphicsAPI::OpenGL:
			//edtInterface_o.ImGuiFrameEnd();
			break;
		};
		Core::GetSystem<vkn::VulkanWin32GraphicsSystem>(); // get command buffer from vulkan
	}
}
