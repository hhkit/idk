#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#include <core/Core.h>
#include <vulkan/VulkanWin32GraphicsSystem.h>
#include <idk_opengl/system/OpenGLGraphicsSystem.h>
#include <win32/WindowsApplication.h>
#include <reflect/ReflectRegistration.h>

#include <gfx/MeshRenderer.h>
#include <scene/SceneManager.h>

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

	c->Setup();

	auto scene = c->GetSystem<SceneManager>().GetActiveScene();
	auto go = scene->CreateGameObject();	
	go->GetComponent<Transform>()->position += vec3{ 0.5, 0.5, 0.0 };
	auto mesh_rend = go->AddComponent<MeshRenderer>();
	
	mesh_rend->material_instance.material = Core::GetResourceManager().Create<Material>("/assets/shader/flat_color.frag");
	c->Run();
	
	auto retval = c->GetSystem<Windows>().GetReturnVal();
	c.reset();
	return retval;
}
