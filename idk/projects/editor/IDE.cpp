#include "pch.h"
#include "IDE.h"

#include <imgui/imgui.h>
#include <vulkan/VulkanWin32GraphicsSystem.h>

namespace idk
{
	void IDE::Init()
	{
		// do imgui stuff

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
		Core::GetSystem<VulkanWin32GraphicsSystem>(); // get command buffer from vulkan
	}
}
