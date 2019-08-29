#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#include <core/Core.h>
#include <vkn/VulkanWin32GraphicsSystem.h>
#include <vkn/VulkanDebugRenderer.h>
#include <idk_opengl/system/OpenGLGraphicsSystem.h>
#include <win32/WindowsApplication.h>
#include <reflect/ReflectRegistration.h>
#include <editor/IDE.h>
#include <file/FileSystem.h>
#include <gfx/MeshRenderer.h>
#include <scene/SceneManager.h>
#include <test/TestComponent.h>

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetBreakAlloc(2455); //To break at a specific allocation number. Useful if your memory leak is consistently at the same spot.
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
    
	using namespace idk;
	
	auto c = std::make_unique<Core>();
	c->AddSystem<Windows>(hInstance, nCmdShow);

	switch (GraphicsAPI::OpenGL)
	{
		case GraphicsAPI::Vulkan:
			c->AddSystem<vkn::VulkanWin32GraphicsSystem>();
			c->AddSystem<vkn::VulkanDebugRenderer>();
			c->AddSystem<IDE>();
			break;
		case GraphicsAPI::OpenGL:
			c->AddSystem<ogl::Win32GraphicsSystem>();
			c->AddSystem<IDE>();
			break;
		default:
			break;
	}

	c->Setup();

	auto scene = c->GetSystem<SceneManager>().GetActiveScene();
	auto go = scene->CreateGameObject();	
	go->AddComponent<TestComponent>();
	go->GetComponent<Transform>()->position += vec3{ 0.5, 0.5, 0.0 };
	//go->GetComponent<Transform>()->scale /= 100.f;
	//go->GetComponent<Transform>()->rotation *= quat{ vec3{0, 0, 1}, deg{90} };
	auto mesh_rend = go->AddComponent<MeshRenderer>();
	
	//mesh_rend->mesh = Core::GetResourceManager().LoadFile(FileHandle{ "/assets/models/boblampclean.md5mesh" }).resources[0].As<Mesh>();
	TestResource t;
	auto m = GenericMetadata{ t };

	static_assert(has_tag_v<TestResource, MetaTag>, "haha");
	string s{ m };

	mesh_rend->material_instance.material = 
		Core::GetResourceManager().LoadFile(Core::GetSystem<FileSystem>().GetFile("/assets/shader/flat_color.frag")).resources[0].As<Material>();
	c->Run();
	
	auto retval = c->GetSystem<Windows>().GetReturnVal();
	c.reset();
	return retval;
}
