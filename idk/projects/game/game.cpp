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

#include <gfx/CameraControls.h>

#include <test/TestSystem.h>
#include <renderdoc/renderdoc_app.h>

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

	RENDERDOC_API_1_1_2* rdoc_api = NULL;

	// At init, on windows
	if (HMODULE mod = LoadLibrary(L"renderdoc.dll"))
	{
		pRENDERDOC_GetAPI RENDERDOC_GetAPI =
			(pRENDERDOC_GetAPI)GetProcAddress(mod, "RENDERDOC_GetAPI");
		RENDERDOC_GetAPI(eRENDERDOC_API_Version_1_1_2, (void**)& rdoc_api);
	}

	using namespace idk;
	
	auto c = std::make_unique<Core>();
	c->AddSystem<Windows>(hInstance, nCmdShow);
	GraphicsSystem* gSys = nullptr;
	auto gfx_api = GraphicsAPI::OpenGL;
	switch (gfx_api)
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
	if (&c->GetSystem<IDE>())
		gSys->editorExist = true;
	c->Setup();

	auto scene = c->GetSystem<SceneManager>().GetActiveScene();
	
	auto camera = scene->CreateGameObject();
	Handle<Camera> camHandle = camera->AddComponent<Camera>();
	camera->GetComponent<Name>()->name = "Camera 1";
	camera->GetComponent<Transform>()->position += vec3{ 0.f, 0.0, -2.5f };
	camHandle->LookAt(vec3(0, 0, 0));
	camHandle->render_target = RscHandle<RenderTarget>{};
	//Core::GetSystem<TestSystem>()->SetMainCamera(camHand);
	float divByVal = 1.f;
	if (&c->GetSystem<IDE>())
	{
		Core::GetSystem<IDE>().currentCamera().current_camera = camHandle;
		divByVal = 200.f;
	}
	auto shader_template = Core::GetResourceManager().LoadFile("/assets/shader/pbr_forward.tmpt")[0].As<ShaderTemplate>();
	auto h_mat = Core::GetResourceManager().Create<Material>();
	h_mat->BuildShader(shader_template, "", "");

	auto createtest_obj = [&scene, h_mat, gfx_api, divByVal](vec3 pos) {
		auto go = scene->CreateGameObject();
		go->GetComponent<Transform>()->position = pos;
		go->Transform()->rotation *= quat{ vec3{1, 0, 0}, deg{-90} };
		go->GetComponent<Transform>()->scale /= divByVal;// 200.f;
		//go->GetComponent<Transform>()->rotation *= quat{ vec3{0, 0, 1}, deg{90} };
		auto mesh_rend = go->AddComponent<MeshRenderer>();
		//Core::GetResourceManager().LoadFile(FileHandle{ "/assets/audio/music/25secClosing_IZHA.wav" });

		//Temp condition, since mesh loader isn't in for vulkan yet
		if (gfx_api != GraphicsAPI::Vulkan)
			mesh_rend->mesh = Core::GetResourceManager().LoadFile(FileHandle{ "/assets/models/boblampclean.md5mesh" })[0].As<Mesh>();
		mesh_rend->material_instance.material = h_mat;
		//mesh_rend->material_instance.uniforms["tex"] = RscHandle <Texture>{};
	};

	createtest_obj(vec3{ 0.5, 0, 0 });
	createtest_obj(vec3{ -0.5, 0, 0 });
	createtest_obj(vec3{ 0, 0, 0 });
	createtest_obj(vec3{ 0, 0.5, 0 });
	createtest_obj(vec3{ 0, -0.5, 0 });

	auto light = scene->CreateGameObject();
	light->AddComponent<Light>();
	light->AddComponent<TestComponent>();
	//light->AddComponent<MeshRenderer>()->mesh = Core::GetResourceManager().LoadFile(FileHandle{ "/assets/models/boblampclean.md5mesh" })[0].As<Mesh>();

	c->Run();
	
	auto retval = c->GetSystem<Windows>().GetReturnVal();
	c.reset();
	return retval;
}
