#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#include <debug/LogSystem.h>
#include <core/Core.h>
#include <core/Scheduler.inl>
#include <PauseConfigurations.h>
#include <vkn/VulkanWin32GraphicsSystem.h>
#include <opengl/system/OpenGLGraphicsSystem.h>
#include <win32/WindowsApplication.h>
#include <win32/XInputSystem.h>
#include <file/FileSystem.h>
#include <gfx/MeshRenderer.h>
#include <scene/SceneManager.h>
#include <test/TestComponent.h>

#include <script/ScriptSystem.h>
#include <script/MonoBehaviorEnvironment.h>
#include <script/MonoBehavior.h>

#include <prefab/Prefab.h>
#include <anim/Animation.h>
#include <anim/Skeleton.h>
#include <serialize/text.h>
#include <res/EasyFactory.h>
#include <res/CompiledAssetLoader.inl>
#include <proj/ProjectManager.h>
#include <scene/scene.h>
#include <res/ResourceHandle.inl>
//#include <editor/loading/AssimpImporter.h>
#include <gfx/GraphFactory.h>
#include <opengl/resource/OpenGLMesh.h>
#include <opengl/resource/OpenGLCubeMapLoader.h>
#include <opengl/resource/OpenGLTextureLoader.h>
//#include <opengl/resource/OpenGLFontAtlasLoader.h>
#include <vkn/VulkanGlslLoader.h>
#include <gfx/ShaderSnippetLoader.h>

#include <vkn/VulkanMesh.h>
#include <vkn/VknFontAtlas.h>

#include <test/TestSystem.h>

#include <network/NetworkSystem.h>

#include "resource.h"
#include <codecvt>
#include <shellapi.h>//CommandLineToArgv

#include <errhandlingapi.h>

#include <vkn/ExtraConfigs.h>

bool overrideMinHide = false;

bool HasArg(std::wstring_view arg, LPWSTR* args, int num_args)
{
	bool result = false;
	for (int i = 0; (i < num_args) & (!result); ++i)
	{
		result |= arg == args[i];
	}
	return result;
}

std::optional<std::wstring_view> GetArgValue(std::wstring_view arg, LPWSTR* args, int num_args)
{
	std::optional<std::wstring_view> result;
	for (int i = 0; (i < num_args) & (!result); ++i)
	{
		std::wstring_view incoming = args[i];
		auto val_start = incoming.find_first_of(':');
		if(val_start!=incoming.npos && incoming.substr(0,val_start)==arg)
			result = incoming.substr(val_start+1);
	}
	return result;
}
LRESULT ProcessAltTab(idk::Windows& win)//, HWND hwnd, UINT msg, WPARAM wParam, [[maybe_unused]] LPARAM lParam)
{
	if (win.GetFullscreen() && !overrideMinHide)
	{
		//bool is_sys_down = msg == WM_SYSKEYDOWN;
		//bool is_sys_up = msg == WM_SYSKEYUP;
		//if ((is_sys_down || is_sys_up) && wParam == VK_TAB)
		//{
		ShowWindow(win.GetWindowHandle(), SW_MINIMIZE);
		//}
	}
	return LRESULT{};
}

void MinimizeOnAltTab(idk::Windows& windows)
{
	static auto on_alt_tab = [&windows]()->LRESULT//HWND hwnd, UINT msg, WPARAM wParam, [[maybe_unused]] LPARAM lParam) -> LRESULT
	{
		return ProcessAltTab(windows);// , hwnd, msg, wParam, lParam);
	};
	windows.OnFocusLost.Listen(on_alt_tab);
}
namespace idk::mt::hack
{
	void SetHelperThreadOverride(int num);
}

//HACK
HWND _hwnd{};

struct ManagedErrorString
{
	ManagedErrorString(DWORD NTStatusMessage, ...);
	ManagedErrorString(DWORD NTStatusMessage, int count = 0, DWORD_PTR* test = nullptr);
	ManagedErrorString(const ManagedErrorString&) = delete; 

	~ManagedErrorString()
	{

		LocalFree(lpMessageBuffer);
		FreeLibrary(Hand);
	}
	LPWSTR lpMessageBuffer{};
	size_t numChars = 0;
	HMODULE Hand{};
};
void RegisterUnhandledExceptionHandler();

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	using namespace idk;
	int num_args = 0;
	auto command_lines = CommandLineToArgvW(lpCmdLine, &num_args);
	//HWND _hwnd{};
	RegisterUnhandledExceptionHandler();

	if (HasArg(L"--single_thd", command_lines, num_args))
	{
		idk::mt::hack::SetHelperThreadOverride(0);
	}
	auto c = std::make_unique<Core>();

	auto& win = c->AddSystem<Windows>(hInstance, nCmdShow, LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON2)));
	_hwnd = win.GetWindowHandle();
	idk::hack::LogSystemConfig::GetSingleton().enabled = HasArg(L"--log", command_lines, num_args);


	if (!HasArg(L"--ignoreAltTabMin", command_lines, num_args))
		MinimizeOnAltTab(win);
	Core::GetSystem<Application>().SetFullscreen(true);
	c->AddSystem<win::XInputSystem>();
	GraphicsSystem* gSys = nullptr;
	auto gfx_api = HasArg(L"--opengl", command_lines, num_args) ? GraphicsAPI::OpenGL : GraphicsAPI::Vulkan;
	switch (gfx_api)
	{
	case GraphicsAPI::Vulkan:
	{
		auto& sys = c->AddSystem<vkn::VulkanWin32GraphicsSystem>();
		if (HasArg(L"--simulate", command_lines, num_args))
		{
			vkn::ExtraConfigs ec{ .enable_simulation=true };
			sys.SetExtraConfigs(ec);
		}
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
	gSys->is_deferred(true);

	namespace fs = std::filesystem;

	fs::path idk_app_data = fs::path{ Core::GetSystem<Application>().GetAppData().c_str() };
	idk_app_data /= "idk";
	if (!fs::exists(idk_app_data))
		fs::create_directory(idk_app_data);

	idk_app_data /= "logs";
	if (!fs::exists(idk_app_data))
		fs::create_directory(idk_app_data);
	Core::GetSystem<LogSystem>().SetLogDir(idk_app_data.string());

	c->Init();

	auto project_dir = string{ Core::GetSystem<FileSystem>().GetExeDir() } +"/project/hydeandseek.idk";
	auto data_dir = string{ Core::GetSystem<FileSystem>().GetExeDir() };
	auto arg_proj = GetArgValue(L"--project", command_lines, num_args);
	auto arg_data = GetArgValue(L"--engine", command_lines, num_args);
	if (arg_proj)
	{
		auto& proj_path = *arg_proj;
		string derp;
		derp.resize(proj_path.length());
		std::locale{ "" };
		std::transform(proj_path.begin(), proj_path.end(), derp.begin(), [](auto wch) {return static_cast<char>(wch); });
		project_dir = derp;
	}
	if (arg_data)
	{
		auto& data_path = *arg_data;
		string derp;
		derp.resize(data_path.length());
		std::locale{ "" };
		std::transform(data_path.begin(), data_path.end(), derp.begin(), [](auto wch) {return static_cast<char>(wch); });
		data_dir = derp;
	}

	Core::GetSystem<ProjectManager>().LoadProject(project_dir);

	Core::GetResourceManager().RegisterLoader<ShaderSnippetLoader>(".glsl");
	switch (gfx_api)
	{
	case GraphicsAPI::Vulkan:
	{
		Core::GetResourceManager().RegisterAssetLoader<CompiledAssetLoader<CompiledMesh, vkn::VulkanMesh>>();
		Core::GetResourceManager().RegisterAssetLoader<CompiledAssetLoader<CompiledTexture, vkn::VknTexture>>();
		Core::GetResourceManager().RegisterAssetLoader<CompiledAssetLoader<CompiledFontAtlas, vkn::VknFontAtlas>>();
		Core::GetResourceManager().RegisterLoader<vkn::VulkanGlslLoader>(".vert");
		Core::GetResourceManager().RegisterLoader<vkn::VulkanGlslLoader>(".frag");
		Core::GetResourceManager().RegisterLoader<vkn::VulkanGlslLoader>(".geom");
		Core::GetResourceManager().RegisterLoader<vkn::VulkanGlslLoader>(".tesc");
		Core::GetResourceManager().RegisterLoader<vkn::VulkanGlslLoader>(".tese");
		Core::GetResourceManager().RegisterLoader<vkn::VulkanGlslLoader>(".comp");
		break;
	}
	case GraphicsAPI::OpenGL:
		Core::GetResourceManager().RegisterAssetLoader<CompiledAssetLoader<CompiledMesh, ogl::OpenGLMesh>>();
		Core::GetResourceManager().RegisterLoader<OpenGLCubeMapLoader>(".cbm");
		Core::GetResourceManager().RegisterLoader<OpenGLTextureLoader>(".png");
		Core::GetResourceManager().RegisterLoader<OpenGLTextureLoader>(".tga");
		Core::GetResourceManager().RegisterLoader<OpenGLTextureLoader>(".jpg");
		Core::GetResourceManager().RegisterLoader<OpenGLTextureLoader>(".jpeg");
		Core::GetResourceManager().RegisterLoader<OpenGLTextureLoader>(".dds");
		//Core::GetResourceManager().RegisterLoader<OpenGLFontAtlasLoader>(".ttf");
		break;
	default:
		break;
	}
	Core::GetResourceManager().RegisterAssetLoader<CompiledAssetLoader<Prefab, Prefab, false>>();
	Core::GetResourceManager().RegisterAssetLoader<CompiledAssetLoader<anim::Animation, anim::Animation>>();
	Core::GetResourceManager().RegisterAssetLoader<CompiledAssetLoader<anim::Skeleton, anim::Skeleton>>();
	Core::GetResourceManager().RegisterLoader<GraphLoader>(shadergraph::Graph::ext);
	Core::GetResourceManager().RegisterFactory<GraphFactory>();

	auto& filesys = Core::GetSystem<FileSystem>();
	filesys.Mount(string{ data_dir } +"/editor_data", "/editor_data", false);
	if (shadergraph::NodeTemplate::GetTable().empty())
		shadergraph::NodeTemplate::LoadTable("/editor_data/nodes");

	c->LateInit();

	for (auto& elem : Core::GetSystem<FileSystem>().GetEntries("/assets", FS_FILTERS::FILE | FS_FILTERS::RECURSE_DIRS))
	{
		if (elem.GetExtension() != ".meta")
			Core::GetResourceManager().Load(elem, false);
	}

	// loaded in ResourceManager::LateInit()
	//for (auto& elem : Core::GetSystem<FileSystem>().GetEntries("/build", FS_FILTERS::FILE | FS_FILTERS::RECURSE_DIRS))
	//	Core::GetResourceManager().LoadCompiledAsset(elem);

	auto load_scene = RscHandle<Scene>{ Guid{ "8574c366-ca08-4da1-ba2e-70e092184dfa" } };
	Core::GetSystem<SceneManager>().SetActiveScene(load_scene);
	load_scene->LoadFromResourcePath();
	Core::GetScheduler().SetPauseState(UnpauseAll);
	Core::GetSystem<mono::ScriptSystem>().run_scripts = true;

	for (int i = 0; i < num_args - 1; ++i)
	{
		if (std::wcscmp(command_lines[i], L"+connect_lobby") == 0)
		{
			auto str = std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(command_lines[i + 1]);
			auto res = parse_text<uint64_t>(str);
			NetworkSystem::connect_lobby_first_frame = res.value_or(0);
			break;
		}
	}

	c->Run();
	return c->GetSystem<Windows>().GetReturnVal();
}

#pragma optimize("",off)
ManagedErrorString::ManagedErrorString(DWORD NTStatusMessage, ...)
{
	va_list list;
	va_start(list, NTStatusMessage);
	Hand = LoadLibrary(L"NTDLL.DLL");


	numChars = FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_FROM_HMODULE | FORMAT_MESSAGE_IGNORE_INSERTS,
		Hand,
		NTStatusMessage,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMessageBuffer,
		0,
		&list);

}
ManagedErrorString::ManagedErrorString(DWORD NTStatusMessage, int count , DWORD_PTR* test )
{
	Hand = LoadLibrary(L"NTDLL.DLL");


	numChars = FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_FROM_HMODULE | FORMAT_MESSAGE_ARGUMENT_ARRAY | FORMAT_MESSAGE_IGNORE_INSERTS,
		Hand,
		NTStatusMessage,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMessageBuffer,
		count,
		(va_list*)test);
}
#define EXTERNAL_EXCEPTION 0xE06D7363
std::wstring GetReadFlag(DWORD flag)
{
	auto isWrite = flag & 0x1;
	auto isDEP = flag & 0x8;
	return std::wstring{ isWrite ? L"written" : L"read" } + std::wstring{ isDEP ? L"(DEP)" : L"" };
}
std::wstring AccessViolationDesc(const _EXCEPTION_RECORD& record)
{
	ManagedErrorString err{ record.ExceptionCode ,0};
	auto flag_msg = GetReadFlag(record.ExceptionInformation[0]);
	if (err.numChars)
	{
		size_t estimated_size = err.numChars + 8 + 8 + flag_msg.size();
		do
		{

		std::wstring result(estimated_size+1,L' ');

		int numCharsWritten = std::swprintf(result.data(), result.size(), err.lpMessageBuffer, record.ExceptionAddress, record.ExceptionInformation[1], flag_msg.c_str());
		if (numCharsWritten > 0)
		{
			result.resize(numCharsWritten);
			return result;
		}
		else
		{
			auto lower_bound = estimated_size;
			lower_bound = 1;
			estimated_size = std::max(estimated_size, lower_bound);
			estimated_size *= 2;
			continue;
		}
		} while (true);
	}
	return L"";
}
std::wstring GetErrorString(const _EXCEPTION_RECORD& record)
{
	std::wstring codeName;
	std::wstring description;
	std::string sdescription;
	switch (record.ExceptionCode)
	{
	case EXTERNAL_EXCEPTION:
		codeName = L"Non-System Exception";
		sdescription = ((std::exception*)record.ExceptionInformation[1])->what();
		break;
		case EXCEPTION_ACCESS_VIOLATION          : codeName = L"EXCEPTION_ACCESS_VIOLATION          "; description = AccessViolationDesc(record); break;
		case EXCEPTION_DATATYPE_MISALIGNMENT     : codeName = L"EXCEPTION_DATATYPE_MISALIGNMENT     "; break;
		case EXCEPTION_BREAKPOINT                : codeName = L"EXCEPTION_BREAKPOINT                "; break;
		case EXCEPTION_SINGLE_STEP               : codeName = L"EXCEPTION_SINGLE_STEP               "; break;
		case EXCEPTION_ARRAY_BOUNDS_EXCEEDED     : codeName = L"EXCEPTION_ARRAY_BOUNDS_EXCEEDED     "; break;
		case EXCEPTION_FLT_DENORMAL_OPERAND      : codeName = L"EXCEPTION_FLT_DENORMAL_OPERAND      "; break;
		case EXCEPTION_FLT_DIVIDE_BY_ZERO        : codeName = L"EXCEPTION_FLT_DIVIDE_BY_ZERO        "; break;
		case EXCEPTION_FLT_INEXACT_RESULT        : codeName = L"EXCEPTION_FLT_INEXACT_RESULT        "; break;
		case EXCEPTION_FLT_INVALID_OPERATION     : codeName = L"EXCEPTION_FLT_INVALID_OPERATION     "; break;
		case EXCEPTION_FLT_OVERFLOW              : codeName = L"EXCEPTION_FLT_OVERFLOW              "; break;
		case EXCEPTION_FLT_STACK_CHECK           : codeName = L"EXCEPTION_FLT_STACK_CHECK           "; break;
		case EXCEPTION_FLT_UNDERFLOW             : codeName = L"EXCEPTION_FLT_UNDERFLOW             "; break;
		case EXCEPTION_INT_DIVIDE_BY_ZERO        : codeName = L"EXCEPTION_INT_DIVIDE_BY_ZERO        "; break;
		case EXCEPTION_INT_OVERFLOW              : codeName = L"EXCEPTION_INT_OVERFLOW              "; break;
		case EXCEPTION_PRIV_INSTRUCTION          : codeName = L"EXCEPTION_PRIV_INSTRUCTION          "; break;
		case EXCEPTION_IN_PAGE_ERROR             : codeName = L"EXCEPTION_IN_PAGE_ERROR             "; break;
		case EXCEPTION_ILLEGAL_INSTRUCTION       : codeName = L"EXCEPTION_ILLEGAL_INSTRUCTION       "; break;
		case EXCEPTION_NONCONTINUABLE_EXCEPTION  : codeName = L"EXCEPTION_NONCONTINUABLE_EXCEPTION  "; break;
		case EXCEPTION_STACK_OVERFLOW            : codeName = L"EXCEPTION_STACK_OVERFLOW            "; break;
		case EXCEPTION_INVALID_DISPOSITION       : codeName = L"EXCEPTION_INVALID_DISPOSITION       "; break;
		case EXCEPTION_GUARD_PAGE                : codeName = L"EXCEPTION_GUARD_PAGE                "; break;
		case EXCEPTION_INVALID_HANDLE            : codeName = L"EXCEPTION_INVALID_HANDLE            "; break;
		case CONTROL_C_EXIT                      : codeName = L"CONTROL_C_EXIT                      "; break;

	default:
		codeName = L"Unknown Exception";
		break;
	}
	std::wstringstream stream;
	stream << std::hex << codeName << L"\nException Code[0x" << record.ExceptionCode << L"] " << L"\nDescription: ";
	if (description.empty())
		stream << sdescription.c_str();
	else
		stream << description;
	stream<< L"\nArgument Values: ";
	for (auto c = record.NumberParameters; c >0; --c)
	{
		auto i = record.NumberParameters - c;
		stream << L"[" << i << L"]: 0x" << std::hex << record.ExceptionInformation[i] << L",";
	}
	//stream <<L"\n"<< (const char*)record.ExceptionInformation;
	return stream.str();
}
void RegisterUnhandledExceptionHandler()
{
	using namespace idk;


	SetUnhandledExceptionFilter([](_In_ struct _EXCEPTION_POINTERS* info) -> LONG {

		Core::GetSystem<LogSystem>().FlushAllLogs();
		auto errorCode = info->ExceptionRecord->ExceptionCode;
		auto errStr = ManagedErrorString{ errorCode,2,info->ExceptionRecord->ExceptionInformation[0],info->ExceptionRecord->ExceptionInformation[1] };// , info->ExceptionRecord->NumberParameters, info->ExceptionRecord->ExceptionInformation };
		overrideMinHide = true;
		auto str = GetErrorString(*info->ExceptionRecord);
		MessageBox(_hwnd, str.c_str(), L"Unhandled Exception", MB_OK);
		auto str2 = GetErrorString(*info->ExceptionRecord);
		auto errStr2 = ManagedErrorString{ errorCode,2,info->ExceptionRecord->ExceptionInformation[0],info->ExceptionRecord->ExceptionInformation[1] };// , info->ExceptionRecord->NumberParameters, info->ExceptionRecord->ExceptionInformation };
		auto errStr3 = ManagedErrorString{ errorCode,2,(DWORD_PTR*)info->ExceptionRecord->ExceptionInformation };// , info->ExceptionRecord->NumberParameters, info->ExceptionRecord->ExceptionInformation };
		return EXCEPTION_EXECUTE_HANDLER;
		});
}