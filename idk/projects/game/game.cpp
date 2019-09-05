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

#include <serialize/serialize.h>

namespace idk
{
	struct yolo
	{
		vector<string> guids;
	};
}
REFLECT_BEGIN(idk::yolo, "yolo")
REFLECT_VAR(guids)
REFLECT_END()

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
	GraphicsSystem* gSys = nullptr;

	switch (GraphicsAPI::OpenGL)
	{
		case GraphicsAPI::Vulkan:
			c->AddSystem<vkn::VulkanWin32GraphicsSystem>();
			c->AddSystem<vkn::VulkanDebugRenderer>();
			c->AddSystem<IDE>();

			gSys = &c->GetSystem<vkn::VulkanWin32GraphicsSystem>();
			break;
		case GraphicsAPI::OpenGL:
			c->AddSystem<ogl::Win32GraphicsSystem>();
			c->AddSystem<IDE>();

			gSys = &c->GetSystem<ogl::Win32GraphicsSystem>();
			break;
		default:
			break;
	}

	c->Setup();

	auto scene = c->GetSystem<SceneManager>().GetActiveScene();
	
	auto camera = scene->CreateGameObject();
	Handle<Camera> camHandle = camera->AddComponent<Camera>();
	camera->GetComponent<Name>()->name = "Camera 1";
	camera->GetComponent<Transform>()->position += vec3{ 0.5, 0.5, 0.0 };
	gSys->SetMainCamera(camHandle);
	
	auto go = scene->CreateGameObject();	
	go->AddComponent<TestComponent>();
	go->GetComponent<Transform>()->position += vec3{ -0.5, 0.5, 0.0 };
	//go->Transform()->rotation *= quat{ vec3{1, 0, 0}, deg{-90} };
	go->GetComponent<Transform>()->scale /= 200.f;
	//go->GetComponent<Transform>()->rotation *= quat{ vec3{0, 0, 1}, deg{90} };
	auto mesh_rend = go->AddComponent<MeshRenderer>();
	//Core::GetResourceManager().LoadFile(FileHandle{ "/assets/audio/music/25secClosing_IZHA.wav" });

	mesh_rend->mesh = Core::GetResourceManager().LoadFile(FileHandle{ "/assets/models/boblampclean.md5mesh" })[0].As<Mesh>();

	auto go2 = scene->CreateGameObject();
	go2->AddComponent<TestComponent>();
	go2->GetComponent<Transform>()->position += vec3{ 0.5, 0.5, 0.0 };
	go2->Transform()->rotation *= quat{ vec3{1, 0, 0}, deg{-90} };
	go2->GetComponent<Transform>()->scale /= 200.f;
	//go->GetComponent<Transform>()->rotation *= quat{ vec3{0, 0, 1}, deg{90} };
	auto mesh_rend2 = go2->AddComponent<MeshRenderer>();
	//Core::GetResourceManager().LoadFile(FileHandle{ "/assets/audio/music/25secClosing_IZHA.wav" });

	mesh_rend2->mesh = Core::GetResourceManager().LoadFile(FileHandle{ "/assets/models/boblampclean.md5mesh" })[0].As<Mesh>();

	auto go3 = scene->CreateGameObject();
	go3->AddComponent<TestComponent>();
	go3->GetComponent<Transform>()->position += vec3{ 0.0, 0.0, 0.0 };
	//go3->Transform()->rotation *= quat{ vec3{1, 0, 0}, deg{-90} };
	go3->GetComponent<Transform>()->scale /= 50.f;
	//go->GetComponent<Transform>()->rotation *= quat{ vec3{0, 0, 1}, deg{90} };
	auto mesh_rend3 = go3->AddComponent<MeshRenderer>();


	auto shader_template = Core::GetResourceManager().LoadFile("/assets/shader/pbr_forward.tmpt")[0].As<ShaderTemplate>();
	auto h_mat = Core::GetResourceManager().Create<Material>();

	h_mat->BuildShader(shader_template, "", "");
	mesh_rend->material_instance.material = h_mat;
	mesh_rend2->material_instance.material = h_mat;
	mesh_rend3->material_instance.material = h_mat;
	c->Run();
	
	auto retval = c->GetSystem<Windows>().GetReturnVal();
	c.reset();
	return retval;
}
