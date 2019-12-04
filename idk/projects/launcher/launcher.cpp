#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#include <debug/LogSystem.h>
#include <core/Core.h>
#include <core/Scheduler.h>
#include <PauseConfigurations.h>
#include <vkn/VulkanWin32GraphicsSystem.h>
#include <opengl/system/OpenGLGraphicsSystem.h>
#include <win32/WindowsApplication.h>
#include <win32/XInputSystem.h>
#include <ReflectRegistration.h>
#include <file/FileSystem.h>
#include <gfx/MeshRenderer.h>
#include <scene/SceneManager.h>
#include <test/TestComponent.h>

#include <script/ScriptSystem.h>
#include <script/MonoBehaviorEnvironment.h>

#include <serialize/text.h>
#include <res/EasyFactory.h>
#include <res/CompiledAssetLoader.h>
#include <editor/loading/AssimpImporter.h>
#include <editor/loading/GraphFactory.h>
#include <opengl/resource/OpenGLMesh.h>
#include <opengl/resource/OpenGLCubeMapLoader.h>
#include <opengl/resource/OpenGLTextureLoader.h>
#include <opengl/resource/OpenGLFontAtlasLoader.h>
#include <vkn/VulkanGlslLoader.h>
#include <gfx/ShaderSnippetLoader.h>

#include <gfx/CameraControls.h>

#include <test/TestSystem.h>

#include <renderdoc/renderdoc_app.h>
#include <shellapi.h>//CommandLineToArgv

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
	using namespace idk;
	int num_args = 0;
	auto command_lines = CommandLineToArgvW(lpCmdLine, &num_args);

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

	auto c = std::make_unique<Core>();

	auto& win = c->AddSystem<Windows>(hInstance, nCmdShow);
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

	namespace fs = std::filesystem;

	fs::path idk_app_data = Core::GetSystem<Application>().GetAppData();
	idk_app_data /= "idk";
	if (!fs::exists(idk_app_data))
		fs::create_directory(idk_app_data);

	idk_app_data /= "logs";
	if (!fs::exists(idk_app_data))
		fs::create_directory(idk_app_data);
	Core::GetSystem<LogSystem>().SetLogDir(idk_app_data.string());

	c->Init();
	Core::GetSystem<ProjectManager>().LoadProject(string{ Core::GetSystem<FileSystem>().GetExeDir() } +"/project/hydeandseek.idk");

	Core::GetResourceManager().RegisterLoader<ShaderSnippetLoader>(".glsl");
	switch (gfx_api)
	{
	case GraphicsAPI::Vulkan:
	{
		Core::GetResourceManager().RegisterLoader<vkn::VulkanGlslLoader>(".vert");
		Core::GetResourceManager().RegisterLoader<vkn::VulkanGlslLoader>(".frag");
		Core::GetResourceManager().RegisterLoader<vkn::VulkanGlslLoader>(".geom");
		Core::GetResourceManager().RegisterLoader<vkn::VulkanGlslLoader>(".tesc");
		Core::GetResourceManager().RegisterLoader<vkn::VulkanGlslLoader>(".tese");
		Core::GetResourceManager().RegisterLoader<vkn::VulkanGlslLoader>(".comp");
	}
	case GraphicsAPI::OpenGL:
		Core::GetResourceManager().RegisterAssetLoader<CompiledAssetLoader<CompiledMesh, ogl::OpenGLMesh, false>>();
		Core::GetResourceManager().RegisterLoader<OpenGLCubeMapLoader>(".cbm");
		Core::GetResourceManager().RegisterLoader<OpenGLTextureLoader>(".png");
		Core::GetResourceManager().RegisterLoader<OpenGLTextureLoader>(".tga");
		Core::GetResourceManager().RegisterLoader<OpenGLTextureLoader>(".jpg");
		Core::GetResourceManager().RegisterLoader<OpenGLTextureLoader>(".jpeg");
		Core::GetResourceManager().RegisterLoader<OpenGLTextureLoader>(".dds");
		Core::GetResourceManager().RegisterLoader<OpenGLFontAtlasLoader>(".ttf");
		break;
	default:
		break;
	}
	Core::GetResourceManager().RegisterAssetLoader<CompiledAssetLoader<Prefab, Prefab, false>>();
	Core::GetResourceManager().RegisterAssetLoader<CompiledAssetLoader<anim::Animation, anim::Animation, false>>();
	Core::GetResourceManager().RegisterAssetLoader<CompiledAssetLoader<anim::Skeleton, anim::Skeleton, false>>();
	Core::GetResourceManager().RegisterLoader<GraphLoader>(shadergraph::Graph::ext);
	Core::GetResourceManager().RegisterFactory<GraphFactory>();

	auto& filesys = Core::GetSystem<FileSystem>();
	filesys.Mount(string{ filesys.GetExeDir() } +"/editor_data", "/editor_data", false);
	if (shadergraph::NodeTemplate::GetTable().empty())
		shadergraph::NodeTemplate::LoadTable("/editor_data/nodes");

	c->LateInit();

	for (auto& elem : Core::GetSystem<FileSystem>().GetEntries("/assets", FS_FILTERS::FILE | FS_FILTERS::RECURSE_DIRS))
	{
		if (elem.GetExtension() != ".meta")
			Core::GetResourceManager().Load(elem, false);
	}

	for (auto& elem : Core::GetSystem<FileSystem>().GetEntries("/build", FS_FILTERS::FILE | FS_FILTERS::RECURSE_DIRS))
		Core::GetResourceManager().LoadCompiledAsset(elem);


	auto load_scene = RscHandle<Scene>{ Guid{ "f0cab184-ac53-4cc4-b191-74a8c65c4c84" } };
	Core::GetSystem<SceneManager>().SetActiveScene(load_scene);
	load_scene->LoadFromResourcePath();
	Core::GetScheduler().SetPauseState(UnpauseAll);
	Core::GetSystem<mono::ScriptSystem>().run_scripts = true;

	c->Run();
	return c->GetSystem<Windows>().GetReturnVal();
}
