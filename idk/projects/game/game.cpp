#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#include <debug/LogSystem.h>
#include <core/Core.h>
#include <vkn/VulkanWin32GraphicsSystem.h>
#include <opengl/system/OpenGLGraphicsSystem.h>
#include <win32/WindowsApplication.h>
#include <win32/XInputSystem.h>
#include <ReflectRegistration.h>
#include <editor/IDE.h>
#include <file/FileSystem.h>
#include <gfx/MeshRenderer.h>
#include <scene/SceneManager.h>
#include <test/TestComponent.h>

#include <script/ScriptSystem.h>
#include <script/MonoBehaviorEnvironment.h>

#include <serialize/text.h>

#include <gfx/CameraControls.h>

#include <test/TestSystem.h>
#include <renderdoc/renderdoc_app.h>

#include <shellapi.h>//CommandLineToArgv

#include <natvis_ids.h>

bool HasArg(std::wstring_view arg, LPWSTR* args, int num_args)
{
	bool result = false;
	for (int i = 0; (i < num_args) & (!result); ++i)
	{
		result |= arg == args[i];
	}
	return result;
}


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	int num_args = 0;
	auto command_lines = CommandLineToArgvW(lpCmdLine, &num_args);
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetBreakAlloc(343170); //To break at a specific allocation number. Useful if your memory leak is consistently at the same spot.
	//_CrtSetBreakAlloc(895766); //To break at a specific allocation number. Useful if your memory leak is consistently at the same spot.
	//_CrtSetBreakAlloc(884500); //To break at a specific allocation number. Useful if your memory leak is consistently at the same spot.
	//_CrtSetBreakAlloc(884499); //To break at a specific allocation number. Useful if your memory leak is consistently at the same spot.
	//_CrtSetBreakAlloc(895231); //To break at a specific allocation number. Useful if your memory leak is consistently at the same spot.

	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	if (!HasArg(L"--nodoc", command_lines, num_args)) //Additional check to allow disabling of renderdoc without changing game.cpp
	{
		RENDERDOC_API_1_1_2* rdoc_api = NULL;

		// At init, on windows
		if (HMODULE mod = LoadLibrary(L"renderdoc.dll"))
		{
			pRENDERDOC_GetAPI RENDERDOC_GetAPI =
				(pRENDERDOC_GetAPI)GetProcAddress(mod, "RENDERDOC_GetAPI");
			RENDERDOC_GetAPI(eRENDERDOC_API_Version_1_1_2, (void**)& rdoc_api);
		}
	}

	using namespace idk;

	auto c = std::make_unique<Core>();
	
    c->AddSystem<Windows>(hInstance, nCmdShow);
    c->AddSystem<win::XInputSystem>();

	GraphicsSystem* gSys = nullptr;
	auto gfx_api = HasArg(L"--vulkan", command_lines, num_args) ? GraphicsAPI::Vulkan : GraphicsAPI::OpenGL;
	switch (gfx_api)
	{
	case GraphicsAPI::Vulkan:
		{
			auto& sys = c->AddSystem<vkn::VulkanWin32GraphicsSystem>();
			gSys = &sys;
			if (HasArg(L"--validation", command_lines, num_args))
				sys.Instance().EnableValidation();
			break;
		}
	case GraphicsAPI::OpenGL: 
		gSys = &c->AddSystem<ogl::Win32GraphicsSystem>();
		break;
	default:
		break;
	
	}
	gSys->is_deferred(HasArg(L"--deferred", command_lines, num_args));

	c->AddSystem<IDE>();

    namespace fs = std::filesystem;

    fs::path idk_app_data = Core::GetSystem<Application>().GetAppData();
    idk_app_data /= "idk";
    if (!fs::exists(idk_app_data))
        fs::create_directory(idk_app_data);

    idk_app_data /= "logs";
    if (!fs::exists(idk_app_data))
        fs::create_directory(idk_app_data);
    Core::GetSystem<LogSystem>().SetLogDir(idk_app_data.string());

	c->Setup();
	c->Run();
	return c->GetSystem<Windows>().GetReturnVal();
}
