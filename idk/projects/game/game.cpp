#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#include <core/Core.h>
#include <vulkan/VulkanWin32GraphicsSystem.h>
#include <idk_opengl/system/OpenGLGraphicsSystem.h>
#include <win32/WindowsApplication.h>
#include <reflect/ReflectRegistration.h>

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
    
	using namespace idk;
	
	auto c = std::make_unique<Core>();
	c->AddSystem<Windows>(hInstance, nCmdShow);

	switch (GraphicsAPI::Default)
	{
		case GraphicsAPI::Vulkan:
			c->AddSystem<VulkanWin32GraphicsSystem>();
			break;
		case GraphicsAPI::OpenGL:
			c->AddSystem<ogl::Win32GraphicsSystem>();
			break;
		default:
			break;
	}
	c->Run();
	
	auto retval = c->GetSystem<Windows>().GetReturnVal();
	c.reset();
	return retval;
}
