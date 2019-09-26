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


#define USE_RENDER_DOC

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetBreakAlloc(8538); //To break at a specific allocation number. Useful if your memory leak is consistently at the same spot.
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
#ifdef USE_RENDER_DOC
	RENDERDOC_API_1_1_2* rdoc_api = NULL;

	// At init, on windows
	if (HMODULE mod = LoadLibrary(L"renderdoc.dll"))
	{
		pRENDERDOC_GetAPI RENDERDOC_GetAPI =
			(pRENDERDOC_GetAPI)GetProcAddress(mod, "RENDERDOC_GetAPI");
		RENDERDOC_GetAPI(eRENDERDOC_API_Version_1_1_2, (void**)& rdoc_api);
	}
#endif
	using namespace idk;
	
	auto c = std::make_unique<Core>();
	c->AddSystem<Windows>(hInstance, nCmdShow);
	GraphicsSystem* gSys = nullptr;
	auto gfx_api = GraphicsAPI::OpenGL;
	switch (gfx_api)
	{
		case GraphicsAPI::Vulkan:
			c->AddSystem<vkn::VulkanWin32GraphicsSystem>();
			//c->AddSystem<IDE>();

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

	//Core::GetResourceManager().LoadFile("/assets/textures/DebugTerrain.png");

	auto scene = c->GetSystem<SceneManager>().GetActiveScene();
	
	auto camera = scene->CreateGameObject();
	Handle<Camera> camHandle = camera->AddComponent<Camera>();
	camera->GetComponent<Name>()->name = "Camera 1";
	camera->GetComponent<Transform>()->position += vec3{ 0.f, 0, 2.5f };
	camHandle->far_plane = 100.f;
	camHandle->LookAt(vec3(0, 0, 0));
	camHandle->render_target = RscHandle<RenderTarget>{};
	camHandle->clear_color = vec4{ 0.05,0.05,0.1,1 };
	//Core::GetSystem<TestSystem>()->SetMainCamera(camHand);
	float divByVal = 2.f;
	if (&c->GetSystem<IDE>())
	{
		Core::GetSystem<IDE>().currentCamera().current_camera = camHandle;
		divByVal = 200.f;
	}
	auto shader_template = *Core::GetResourceManager().Load<ShaderTemplate>("/assets/shader/pbr_forward.tmpt");
	auto h_mat = Core::GetResourceManager().Create<Material>();
	h_mat->BuildShader(shader_template, "", "");

	// Lambda for creating an animated object... Does not work atm.
	auto create_anim_obj = [&scene, h_mat, gfx_api, divByVal](vec3 pos) {
		auto go = scene->CreateGameObject();
		go->GetComponent<Transform>()->position = pos;
		// go->Transform()->rotation *= quat{ vec3{1, 0, 0}, deg{-90} };
		go->GetComponent<Transform>()->scale /= 200;// 200.f;
		// go->GetComponent<Transform>()->rotation *= quat{ vec3{0, 0, 1}, deg{90} };
		auto mesh_rend = go->AddComponent<SkinnedMeshRenderer>();
		auto animator = go->AddComponent<AnimationController>();

		//Temp condition, since mesh loader isn't in for vulkan yet
		if (gfx_api != GraphicsAPI::Vulkan)
		{
			auto resources = Core::GetResourceManager().Load(PathHandle{ "/assets/models/Running.fbx" });
			mesh_rend->mesh = resources->Get<Mesh>();
			animator->SetSkeleton(resources->Get<anim::Skeleton>());
			animator->AddAnimation(resources->Get<anim::Animation>());
			animator->Play(0);
		}
		mesh_rend->material_instance.material = h_mat;

		return go;
	};
	auto tmp_tex = RscHandle<Texture>{};
	if(gfx_api == GraphicsAPI::Vulkan)
		tmp_tex = *Core::GetResourceManager().Load<Texture>(PathHandle{ "/assets/textures/texture.dds" });

	constexpr auto col = ivec3{ 1,0,0 };

	// @Joseph: Uncomment this when testing.
	 create_anim_obj(vec3{ 0,0,0 });

	auto createtest_obj = [&scene, h_mat, gfx_api, divByVal,tmp_tex](vec3 pos) {
		auto go = scene->CreateGameObject();
		go->GetComponent<Transform>()->position = pos;
		go->Transform()->rotation *= quat{ vec3{1, 0, 0}, deg{-90} }; 
		go->GetComponent<Transform>()->scale = vec3{ 1 / 5.f };
		//go->GetComponent<Transform>()->rotation *= quat{ vec3{0, 0, 1}, deg{90} };
		auto mesh_rend = go->AddComponent<MeshRenderer>();
		//Core::GetResourceManager().LoadFile(PathHandle{ "/assets/audio/music/25secClosing_IZHA.wav" });

		//Temp condition, since mesh loader isn't in for vulkan yet
		//if (gfx_api != GraphicsAPI::Vulkan)
		//	mesh_rend->mesh = Core::GetResourceManager().LoadFile(PathHandle{ "/assets/models/boblampclean.md5mesh" })[0].As<Mesh>();
		//mesh_rend->mesh = Mesh::defaults[MeshType::Sphere];
		mesh_rend->material_instance.material = h_mat;
		mesh_rend->material_instance.uniforms["tex"] = tmp_tex;

		return go;
	};

	createtest_obj(vec3{ 0.5, 0, 0 });
	createtest_obj(vec3{ -0.5, 0, 0 });
	//createtest_obj(vec3{ 0, 0.5, 0 });
	//createtest_obj(vec3{ 0, -0.5, 0 });

	auto floor = scene->CreateGameObject();
	floor->Transform()->position = vec3{ 0, -1, 0 };
	//floor->Transform()->rotation = quat{ vec3{0,1,0}, deg{45} };
	floor->Transform()->scale    = vec3{ 10, 2, 10 };
	floor->AddComponent<Collider>()->shape = box{};
	{
		auto wall = scene->CreateGameObject();
		wall->Transform()->position = vec3{ 5, 5, 0 };
		wall->Transform()->scale = vec3{ 2, 10, 10 };
		wall->AddComponent<Collider>()->shape = box{};
	}
	{
		auto wall = scene->CreateGameObject();
		wall->Transform()->position = vec3{ -5, 5, 0 };
		wall->Transform()->scale = vec3{ 2, 10, 10 };
		wall->AddComponent<Collider>()->shape = box{};
	}
	{
		auto wall = scene->CreateGameObject();
		wall->Transform()->position = vec3{ 0, 5, 5 };
		wall->Transform()->scale = vec3{ 10, 10, 2 };
		wall->AddComponent<Collider>()->shape = box{};
	}
	{
		auto wall = scene->CreateGameObject();
		wall->Transform()->position = vec3{ 0, 5, -5 };
		wall->Transform()->scale = vec3{ 10, 10, 2 };
		wall->AddComponent<Collider>()->shape = box{};
	}

	auto light = scene->CreateGameObject();
	light->GetComponent<Transform>()->position = vec3{ 0,0,0.0f };
	light->AddComponent<Light>();
	light->AddComponent<TestComponent>();
	//light->AddComponent<MeshRenderer>()->mesh = Core::GetResourceManager().LoadFile(PathHandle{ "/assets/models/boblampclean.md5mesh" })[0].As<Mesh>();
	//auto bounce_kun = scene->CreateGameObject();
	//bounce_kun->GetComponent<Name>()->name = "bouncer";
	//bounce_kun->Transform()->position = vec3{ 0, 1, 0 };
	//bounce_kun->Transform()->rotation = quat{ vec3{0,1,0}, deg{45} };
	//bounce_kun->Transform()->scale = vec3{ 1.f / 4 };
	//bounce_kun->AddComponent<RigidBody>();
	//bounce_kun->AddComponent<Light>();
	//bounce_kun->AddComponent<Collider>()->shape = box{};
	//bounce_kun->AddComponent<TestComponent>();

	/* physics resolution demonstration */
	{
		auto seduceme = scene->CreateGameObject();
		seduceme->GetComponent<Name>()->name = "seduceme";
		seduceme->Transform()->position = vec3{ 0, 0.125, 0 };
		//seduceme->Transform()->rotation = quat{ vec3{0,1,0}, deg{30} } *quat{ vec3{1,0,0},  deg{30} };
		seduceme->Transform()->rotation = quat{ vec3{1,1,0}, deg{30} };
		seduceme->Transform()->scale = vec3{ 1.f / 4 };
		seduceme->AddComponent<RigidBody>(); // ->initial_velocity = vec3{ 0.1, 0, 0 };
		seduceme->AddComponent<Collider>()->shape = sphere{};
	}
	{
		auto seducer = scene->CreateGameObject();
		seducer->GetComponent<Name>()->name = "seducer";
		seducer->Transform()->position = vec3{ -2, 0.125, 0 };
		seducer->Transform()->scale = vec3{ 1.f / 4 };
		seducer->AddComponent<RigidBody>()->initial_velocity = vec3{  1, 0, 0 };
		seducer->AddComponent<Collider>()->shape = sphere{};
	}
	{
		auto seducer = scene->CreateGameObject();
		seducer->GetComponent<Name>()->name = "seducer";
		seducer->Transform()->position = vec3{ 2, 0.125, 0 };
		seducer->Transform()->scale = vec3{ 1.f / 4 };
		seducer->AddComponent<RigidBody>()->initial_velocity = vec3{ -1, 0, 0 };
		seducer->AddComponent<Collider>()->shape = sphere{};
	}

	//if(0)
	for (int i = 2; i < 5; ++ i)
	{
		auto seducemetoo = scene->CreateGameObject();
		seducemetoo->GetComponent<Name>()->name = "seducemetoo";
		seducemetoo->Transform()->position = vec3{ 0, i, 0 };
		seducemetoo->Transform()->rotation = quat{ vec3{1,1,0}, deg{30} };
		seducemetoo->Transform()->scale = vec3{ 1.f / 4 };
		seducemetoo->AddComponent<RigidBody>();
		seducemetoo->AddComponent<Collider>()->shape = box{};
	}
	c->Run();
	
	auto retval = c->GetSystem<Windows>().GetReturnVal();
	c.reset();
	return retval;
}
