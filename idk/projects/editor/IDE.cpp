#include "pch.h"
#include "IDE.h"

#include <imgui/imgui.h>
#include <vkn/VulkanWin32GraphicsSystem.h>
#include <vkn/VulkanState.h>
#include <loading/FBXLoader.h>

namespace idk
{
	IDE::IDE():edtInterface{nullptr}
	{
	}

	void IDE::Init()
	{
		// do imgui stuff
		vkn::VulkanWin32GraphicsSystem * vksys = &Core::GetSystem<vkn::VulkanWin32GraphicsSystem>();
		vkn::VulkanState* inst = &vksys->Instance();
		edtInterface = edt::VI_Interface{ inst };
		edtInterface.Init();

		switch (Core::GetSystem<GraphicsSystem>().GetAPI())
		{
		case GraphicsAPI::OpenGL:
			Core::GetResourceManager().RegisterExtensionLoader<OpenGLFBXLoader>(".fbx");
		default:
			break;
		}
	}

	void IDE::Shutdown()
	{
		// close imgui stuff
		edtInterface.Shutdown();
	}

	void IDE::EditorUpdate()
	{
		// update editor vars
		edtInterface.ImGuiFrameUpdate();
	}

	void IDE::EditorDraw()
	{
		// call imgui draw,
		//edtInterface.ImGuiFrameEnd();
		Core::GetSystem<vkn::VulkanWin32GraphicsSystem>(); // get command buffer from vulkan
	}
}
