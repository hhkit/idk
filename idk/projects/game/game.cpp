#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#include <core/Core.h>
#include <vkn/VulkanWin32GraphicsSystem.h>
#include <vkn/VulkanDebugRenderer.h>
#include <idk_opengl/OpenGLGraphicsSystem.h>
#include <win32/WindowsApplication.h>
#include <reflect/ReflectRegistration.h>
#include <editor/IDE.h>

enum class GraphicsLibrary
{
	OpenGL,
	Vulkan,
	Default = Vulkan
};

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
    
	using namespace idk;
	
	auto c = Core{};
	auto& wind = c.AddSystem<Windows>(hInstance, nCmdShow);

	switch (GraphicsLibrary::Default)
	{
		case GraphicsLibrary::Vulkan:
		{
			auto& gfx_sys = c.AddSystem<vkn::VulkanWin32GraphicsSystem>(wind);
			c.AddSystem<vkn::VulkanDebugRenderer>(gfx_sys.Instance());
			c.AddSystem<IDE>(gfx_sys.Instance());
		}
			break;
		case GraphicsLibrary::OpenGL:
			c.AddSystem<ogl::Win32GraphicsSystem>();
			break;
		default:
			break;
	}
	c.Run();
	
	return c.GetSystem<Windows>().GetReturnVal();
}
