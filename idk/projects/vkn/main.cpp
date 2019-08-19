/*
#include <WindowsWindow.h>
#include <WindowsProgram.h>
#include <ISubProgram.h>
#include <WinClassEx.inl>
#include <optional>
#include "Vulkan.h"

struct TmpSubProgram : ISubProgram
{
	Vulkan* vulkan;
	void Init()	                          override {}
	bool Update()                         override;// { return true; }
	void Free()                           override {}
	void ProcessMessage(WinMsg const&)override
	{
	}
};

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE               //hPrevInstance,
	, _In_ LPWSTR                      //lpCmdLine,
	, _In_ int                         //nCmdShow
)
{
	TmpSubProgram tp;
	WindowsProgram wp(hInstance, &tp, L"Vulkan Test Proj", L"Vulkan Test Proj");
	WinClassEx wc;
	wp.RegisterWindowClass(wc);
	std::shared_ptr<WindowsWindow> window = wp.NewWindow("Vulkan Test Proj NW", true);
	{
		Vulkan vulk;
		Vulkan vulk2;
		window->ShowConsole();
		vulk.InitVulkanEnvironment(wp, window);
		vulk.m_window = window;
		window->OnResize = [&vulk](int x, int y) { utl::cerr() << x << ", " << y << std::endl; vulk.OnResize(); };
		//vulk2.InitVulkanEnvironment(wp, window);
		tp.vulkan = &vulk;
		window->BringToFront();
		std::cout << "Initialization Complete." << std::endl;
		wp.Loop();
		vulk.Cleanup();
		//vulk2.Cleanup();
	}
	system("pause");
	return 0;
}

bool TmpSubProgram::Update()
{
	vulkan->DrawFrame();
	return true;
}
*/