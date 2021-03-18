#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#include <core/Core.h>
#include <core/Core.inl>
#include <debug/LogSystem.h>
#include <win32/WindowsApplication.h>



#include <renderdoc/renderdoc_app.h>

#include <shellapi.h>//CommandLineToArgv

#include <natvis_ids.h>
#include <errhandlingapi.h>

bool HasArg(std::wstring_view arg, LPWSTR* args, int num_args)
{
	bool result = false;
	for (int i = 0; (i < num_args) & (!result); ++i)
	{
		result |= arg == args[i];
	}
	return result;
}
void AddSystems(idk::unique_ptr<idk::Core>& c, HINSTANCE hInstance, int nCmdShow, LPWSTR* command_lines, int num_args);
void ConfigAppDataAndLogger();

RENDERDOC_API_1_1_2*& GetRDocApi()
{
	static RENDERDOC_API_1_1_2* ptr;
	return ptr;
}
namespace idk::mt::hack
{

	void SetHelperThreadOverride(int num);
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
	SetUnhandledExceptionFilter([](_In_ struct _EXCEPTION_POINTERS* info) -> LONG {
		idk::Core::GetSystem<idk::LogSystem>().FlushAllLogs();
		return EXCEPTION_EXECUTE_HANDLER;
		});

	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	if (HasArg(L"--rdoc", command_lines, num_args)) //Additional check to allow disabling of renderdoc without changing game.cpp
	{
		RENDERDOC_API_1_1_2* rdoc_api = NULL;

		// At init, on windows
		if (HMODULE mod = LoadLibrary(L"renderdoc.dll"))
		{
			pRENDERDOC_GetAPI RENDERDOC_GetAPI =
				(pRENDERDOC_GetAPI)GetProcAddress(mod, "RENDERDOC_GetAPI");
			RENDERDOC_GetAPI(eRENDERDOC_API_Version_1_1_2, (void**)& rdoc_api);
			GetRDocApi() = rdoc_api;
		}
	}

	if (HasArg(L"--single_thd", command_lines, num_args))
	{
		idk::mt::hack::SetHelperThreadOverride(0);
	}
	using namespace idk;

	auto c = std::make_unique<Core>();
	try
	{

		AddSystems(c, hInstance, nCmdShow, command_lines, num_args);

		ConfigAppDataAndLogger();

		c->Setup();
		c->Run();
	}
	catch (...)
	{

	}

	return c->GetSystem<Windows>().GetReturnVal();
}
