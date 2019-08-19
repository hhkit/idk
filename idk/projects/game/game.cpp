#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#include <core/Core.h>
#include <vkn/VulkanWin32GraphicsSystem.h>
#include <vkn/VulkanDebugRenderer.h>
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
	auto c = Core{};
	auto& wind = c.AddSystem<Windows>(hInstance, nCmdShow);
	auto& gfx_sys = c.AddSystem<idk::VulkanWin32GraphicsSystem>(wind);
	c.AddSystem<idk::VulkanDebugRenderer>(gfx_sys.Instance());
	c.Run();
	
	return c.GetSystem<Windows>().GetReturnVal();
}
