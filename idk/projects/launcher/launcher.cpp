#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#include <debug/LogSystem.h>
#include <core/Core.h>
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

#include <gfx/CameraControls.h>

#include <test/TestSystem.h>

#include "SomeHackyThing.h"


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	using namespace idk;

	auto c = std::make_unique<Core>();

	auto& win = c->AddSystem<Windows>(hInstance, nCmdShow);
	c->AddSystem<win::XInputSystem>();
	c->AddSystem<ogl::Win32GraphicsSystem>();
	c->AddSystem<SomeHackyThing>();
	c->Run();
	return c->GetSystem<Windows>().GetReturnVal();
}
