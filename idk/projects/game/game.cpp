#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#include <core/Core.h>
#include <vulkan/VulkanWin32GraphicsSystem.h>
#include <idk_opengl/OpenGLGraphicsSystem.h>
#include <win32/WindowsApplication.h>
#include <reflect/ReflectRegistration.h>

enum class GraphicsLibrary
{
	OpenGL,
	Vulkan,
	Default = OpenGL
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
	c.AddSystem<Windows>(hInstance, nCmdShow);

	switch (GraphicsLibrary::Default)
	{
		case GraphicsLibrary::Vulkan:
			c.AddSystem<VulkanWin32GraphicsSystem>();
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
