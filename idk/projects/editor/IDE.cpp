#include "pch.h"
#include "IDE.h"

#include <imgui/imgui.h>
#include <vkn/VulkanWin32GraphicsSystem.h>

namespace idk
{
	void IDE::Init()
	{
		// do imgui stuff
		vkn::VulkanWin32GraphicsSystem * vksys = &Core::GetSystem<vkn::VulkanWin32GraphicsSystem>();
		vkn::VulkanState* inst = &vksys->Instance();
		
		VIInterface_Init(inst);
	}

	void IDE::Shutdown()
	{
		// close imgui stuff
	}

	void IDE::EditorUpdate()
	{
		// update editor vars
	}

	void IDE::EditorDraw()
	{
		// call imgui draw,
		Core::GetSystem<vkn::VulkanWin32GraphicsSystem>(); // get command buffer from vulkan
	}
}
