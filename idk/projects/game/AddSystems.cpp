#include <pch_common.h>
#include <core/Core.inl>
#include <win32/WindowsApplication.h>
#include <win32/XInputSystem.h>

#include <vkn/VulkanWin32GraphicsSystem.h>
#include <opengl/system/OpenGLGraphicsSystem.h>
#include <editor/IDE.h>
#include <file/FileSystem.h>
#include <debug/LogSystem.h>

bool HasArg(std::wstring_view arg, LPWSTR* args, int num_args);

void AddSystems(idk::unique_ptr<idk::Core>& c, HINSTANCE hInstance, int nCmdShow, LPWSTR* command_lines, int num_args)
{
	using namespace idk;
    c->AddSystem<Windows>(hInstance, nCmdShow).SetFullscreen(false);
	c->AddSystem<win::XInputSystem>();

	GraphicsSystem* gSys = nullptr;
	auto gfx_api = HasArg(L"--opengl", command_lines, num_args) ? GraphicsAPI::OpenGL : GraphicsAPI::Vulkan;
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
	gSys->is_deferred(!HasArg(L"--forward", command_lines, num_args));

	c->AddSystem<IDE>();

}
void ConfigAppDataAndLogger()
{
	using namespace idk;
	namespace fs = std::filesystem;

	fs::path idk_app_data = Core::GetSystem<Application>().GetAppData().sv();
	idk_app_data /= "idk";
	if (!fs::exists(idk_app_data))
		fs::create_directory(idk_app_data);

	idk_app_data /= "logs";
	if (!fs::exists(idk_app_data))
		fs::create_directory(idk_app_data);
	Core::GetSystem<LogSystem>().SetLogDir(idk_app_data.string());
}