#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#include <core/Core.h>
#include <vkn/VulkanWin32GraphicsSystem.h>
#include <vkn/VulkanDebugRenderer.h>
#include <idk_opengl/system/OpenGLGraphicsSystem.h>
#include <win32/WindowsApplication.h>
#include <win32/XInputSystem.h>
#include <ReflectRegistration.h>
#include <editor/IDE.h>
#include <file/FileSystem.h>
#include <gfx/MeshRenderer.h>
#include <scene/SceneManager.h>
#include <test/TestComponent.h>
#include <math/euler_angles.h>

#include <editor/loading/OpenGLCubeMapLoader.h>

#include <serialize/serialize.h>

#include <gfx/CameraControls.h>

#include <test/TestSystem.h>
#include <renderdoc/renderdoc_app.h>

#include <shellapi.h>//CommandLineToArgv

#define USE_RENDER_DOC

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
	int num_args = 0;
	auto command_lines = CommandLineToArgvW(lpCmdLine, &num_args);
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetBreakAlloc(102284); //To break at a specific allocation number. Useful if your memory leak is consistently at the same spot.
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
#ifdef USE_RENDER_DOC
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
#endif
	using namespace idk;

	auto c = std::make_unique<Core>();
	
    auto& win = c->AddSystem<Windows>(hInstance, nCmdShow);
    c->AddSystem<win::XInputSystem>();

	GraphicsSystem* gSys = nullptr;
	auto gfx_api = GraphicsAPI::OpenGL;
	switch (gfx_api)
	{
	case GraphicsAPI::Vulkan:
	{
		auto sys = &c->AddSystem<vkn::VulkanWin32GraphicsSystem>();

		c->AddSystem<IDE>();
		win.OnScreenSizeChanged.Listen([sys](const ivec2&) { sys->Instance().OnResize(); });
		gSys = &c->GetSystem<vkn::VulkanWin32GraphicsSystem>();
	}
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
	gSys->brdf = *Core::GetResourceManager().Load<ShaderProgram>("/assets/shader/brdf.frag");
	gSys->convoluter = *Core::GetResourceManager().Load<ShaderProgram>("/assets/shader/pbr_convolute.frag");

	Core::GetResourceManager().Create<TestResource>("/assets/test/yolo.test");

	auto minecraft_texture = *Core::GetResourceManager().Load<Texture>("/assets/textures/DebugTerrain.png");

	auto scene = c->GetSystem<SceneManager>().GetActiveScene();

	float divByVal = 2.f;
	{
		auto camera = scene->CreateGameObject();
		Handle<Camera> camHandle = camera->AddComponent<Camera>();
		camera->GetComponent<Name>()->name = "Camera 1";
		camera->Transform()->position = vec3{ -1.937, 0.707, 4.437 };
		euler_angles euler;
		euler.x = deg{ 1 };
		euler.y = deg{ -24 };
		euler.z = deg{};
		camera->Transform()->rotation = s_cast<quat>(euler);
		camHandle->far_plane = 100.f;
		//camHandle->LookAt(vec3(0, 0, 0));
		camHandle->render_target = RscHandle<RenderTarget>{};
		//camHandle->is_orthographic = true;
		//camHandle->orthographic_size = 10.f;
		//camHandle->render_target->AddAttachment(eDepth);
		camHandle->clear = color{ 0.05,0.05,0.1,1 };
		if(gfx_api!=GraphicsAPI::Vulkan)
			camHandle->clear = *Core::GetResourceManager().Load<CubeMap>("/assets/textures/skybox/space.png.cbm");
		//auto mesh_rend = camera->AddComponent<MeshRenderer>();

		//Core::GetSystem<TestSystem>()->SetMainCamera(camHand);
		if (&c->GetSystem<IDE>())
		{
			Core::GetSystem<IDE>().currentCamera().current_camera = camHandle;
			divByVal = 200.f;
		}
	}
	auto mat_inst = Core::GetResourceManager().Create<MaterialInstance>();
	mat_inst->material = Core::GetResourceManager().Load<shadergraph::Graph>("/assets/materials/test.mat").value();
	mat_inst->uniforms["tex"] = minecraft_texture;

	auto& mat_inst_v = mat_inst->material.as<Material>();
	auto& shader_v = mat_inst->material->_shader_program.as<ShaderProgram>();;

	// Lambda for creating an animated object... Does not work atm.
	auto create_anim_obj = [&scene, mat_inst, gfx_api, divByVal](vec3 pos, PathHandle path = PathHandle{ "/assets/models/Calico Cat.fbx" }) {
		auto go = scene->CreateGameObject();

		go->Name(path.GetStem());
		go->GetComponent<Transform>()->position = pos;
		go->GetComponent<Transform>()->scale /= 100.0f;
		auto animator = go->AddComponent<Animator>();

		//Temp condition, since mesh loader isn't in for vulkan yet
		if (gfx_api != GraphicsAPI::Vulkan)
		{
			auto resources_running = Core::GetResourceManager().Load(path);

			for (auto handle : resources_running->GetAll<Mesh>())
			{
				auto mesh_child_go = scene->CreateGameObject();

				mesh_child_go->Name(handle->Name());
				mesh_child_go->Transform()->parent = go;

				auto mesh_rend = mesh_child_go->AddComponent<SkinnedMeshRenderer>();
				mesh_rend->mesh = handle;
				mesh_rend->material_instance = mat_inst;
			}

			animator->SetSkeleton(resources_running->Get<anim::Skeleton>());

			for (auto& anim : resources_running->GetAll<anim::Animation>())
			{
				animator->AddAnimation(anim);
			}
		}
		return go;
	};

	auto tmp_tex = minecraft_texture;
	if (gfx_api == GraphicsAPI::Vulkan)
		tmp_tex = *Core::GetResourceManager().Load<Texture>(PathHandle{ "/assets/textures/texture.dds" });

	constexpr auto col = ivec3{ 1,0,0 };

	// @Joseph: Uncomment this when testing.
	create_anim_obj(vec3{ 1.796,0,-1.781 });

	auto createtest_obj = [&scene, mat_inst, gfx_api, divByVal, tmp_tex](vec3 pos) {
		auto go = scene->CreateGameObject();
		go->Name("test");
		go->GetComponent<Transform>()->position = pos;
		go->Transform()->rotation *= quat{ vec3{1, 0, 0}, deg{-90} };
		go->GetComponent<Transform>()->scale = vec3{ 1 / 5.f };
		//go->GetComponent<Transform>()->rotation *= quat{ vec3{0, 0, 1}, deg{90} };
		auto mesh_rend = go->AddComponent<MeshRenderer>();
		//Core::GetResourceManager().LoadFile(PathHandle{ "/assets/audio/music/25secClosing_IZHA.wav" });

		//Temp condition, since mesh loader isn't in for vulkan yet
		//if (gfx_api != GraphicsAPI::Vulkan)
		//mesh_rend->mesh = Core::GetResourceManager().LoadFile(PathHandle{ "/assets/models/boblampclean.md5mesh" })[0].As<Mesh>();
		mesh_rend->mesh = Mesh::defaults[MeshType::Sphere];
		mesh_rend->material_instance = mat_inst;

		return go;
	};

	//createtest_obj(vec3{ 0.5, 0, 0 });
	//createtest_obj(vec3{ -0.5, 0, 0 });
	//createtest_obj(vec3{ 0, 0.5, 0 });
	//createtest_obj(vec3{ 0, -0.5, 0 });
	{
		auto floor = scene->CreateGameObject();
		floor->Name("floor");
		floor->Transform()->position = vec3{ 0, -1, 0 };
		//floor->Transform()->rotation = quat{ vec3{0,1,0}, deg{45} };
		floor->Transform()->scale = vec3{ 10, 2, 10 };
		floor->AddComponent<Collider>()->shape = box{};
		auto mesh_rend = floor->AddComponent<MeshRenderer>();
		mesh_rend->mesh = Mesh::defaults[MeshType::Plane];
		mesh_rend->material_instance = mat_inst;
		mesh_rend->material_instance->uniforms["tex"] = *Core::GetResourceManager().Load<Texture>(PathHandle{ "/assets/textures/Grass.jpg" });
	}
	{
		auto wall = scene->CreateGameObject();
		wall->Name("wall");
		wall->Transform()->position = vec3{ 5, 5, 0 };
		wall->Transform()->scale = vec3{ 2, 10, 10 };
		wall->AddComponent<Collider>()->shape = box{};
	}
	{
		auto wall = scene->CreateGameObject();
		wall->Name("wall");
		wall->Transform()->position = vec3{ -5, 5, 0 };
		wall->Transform()->scale = vec3{ 2, 10, 10 };
		wall->AddComponent<Collider>()->shape = box{};
	}
	{
		auto wall = scene->CreateGameObject();
		wall->Name("wall");
		wall->Transform()->position = vec3{ 0, 5, 5 };
		wall->Transform()->scale = vec3{ 10, 10, 2 };
		wall->AddComponent<Collider>()->shape = box{};
	}
	{
		auto wall = scene->CreateGameObject();
		wall->Name("wall");
		wall->Transform()->position = vec3{ 0, 5, -5 };
		wall->Transform()->scale = vec3{ 10, 10, 2 };
		wall->AddComponent<Collider>()->shape = box{};
	}
	if(0)
	{
		auto light = scene->CreateGameObject();
		light->Name("Point Light");
		light->GetComponent<Transform>()->position = vec3{ -1.546f, 1.884f,-0.448f };
		auto light_comp = light->AddComponent<Light>();
		{
			//auto light_map = Core::GetResourceManager().Create<RenderTarget>();
			//light_comp->SetLightMap(light_map);
		}
		light_comp->light = PointLight{
			real{1.f},
			color{0.8,0,0}
		};
		light->AddComponent<TestComponent>();
	}

	{
		auto light = scene->CreateGameObject();
		light->Name("Directional Light");
		light->GetComponent<Transform>()->position = vec3{ 1.749,1.699,1.054f };
		euler_angles rot;
		rot.x = deg{ 94 };
		rot.y = deg{ -9 };
		rot.z = deg{ -49 };
		light->Transform()->rotation = s_cast<quat>(rot);
		auto light_comp = light->AddComponent<Light>();
		{
			light_comp->light = DirectionalLight{
				real{1.f},
				color{0.5,0,1}
			};
			auto light_map = Core::GetResourceManager().Create<RenderTarget>();
			auto m = light_map->GetMeta().textures[0]->GetMeta();
			//m.internal_format = ColorFormat::DEPTH_COMPONENT;
			//m.format = InputChannels::DEPTH_COMPONENT;
			m.filter_mode = FilterMode::Nearest;
			m.uv_mode = UVMode::ClampToBorder;
			//light_map->GetMeta().textures[0]->Size(ivec2{ 1024,1024 });
			light_map->GetMeta().textures[0]->SetMeta(m);
			light_comp->SetLightMap(light_map);
		}
		light->AddComponent<TestComponent>();
	}
	if (0)
	{
		auto light = scene->CreateGameObject();
		light->Name("SpotLight");
		light->GetComponent<Transform>()->position = vec3{ 0,0,0.0f };
		auto light_comp = light->AddComponent<Light>();
		{
			auto light_map = Core::GetResourceManager().Create<RenderTarget>();
			auto light_obj = SpotLight{};
			//light_obj.inner_angle = rad{ 0.5f };
			light_obj.attenuation_radius = 0.1f;
			light_comp->light = light_obj;
			light_comp->SetLightMap(light_map);
		}
		light->AddComponent<TestComponent>();
	}
	if (0)
	{
		auto light = scene->CreateGameObject();
		light->Name("Point Light 2");
		light->GetComponent<Transform>()->position = vec3{ 1.98,1.521,-1.431f };
		auto light_comp = light->AddComponent<Light>();
		light_comp->light = PointLight{
			real{1.f},
			color{1,1,1}
		};
		{
			auto light_map = Core::GetResourceManager().Create<RenderTarget>();
			light_comp->SetLightMap(light_map);
		}
		light->AddComponent<TestComponent>();
	}
	/* physics resolution demonstration */
	 {
	 	auto seduceme = scene->CreateGameObject();
	 	seduceme->GetComponent<Name>()->name = "seduceme";
	 	seduceme->Transform()->position = vec3{ 0, 0.125, 0 };
	 	//seduceme->Transform()->rotation = quat{ vec3{0,1,0}, deg{30} } *quat{ vec3{1,0,0},  deg{30} };
	 	seduceme->Transform()->rotation = quat{ vec3{1,1,0}, deg{30} };
	 	seduceme->Transform()->scale = vec3{ 1.f / 4 };
	 	seduceme->AddComponent<RigidBody>();
	 	auto mesh_rend = seduceme->AddComponent<MeshRenderer>();
	 	mesh_rend->mesh = Mesh::defaults[MeshType::Sphere];
	 	mesh_rend->material_instance = mat_inst;
	 	seduceme->AddComponent<Collider>()->shape = sphere{ vec3{}, 1 };
	 }
	 {
	 	auto seducer = scene->CreateGameObject();
	 	seducer->GetComponent<Name>()->name = "seducer";
	 	seducer->Transform()->position = vec3{ -2, 0.125, 0 };
	 	seducer->Transform()->scale = vec3{ 1.f / 4 };
	 	seducer->AddComponent<RigidBody>()->initial_velocity = vec3{  2, 0, 0 };
	 	auto mesh_rend = seducer->AddComponent<MeshRenderer>();
	 	mesh_rend->mesh = Mesh::defaults[MeshType::Sphere];
	 	mesh_rend->material_instance = mat_inst;
	 	seducer->AddComponent<Collider>()->shape = sphere{ vec3{}, 1 };
	 }
	 {
	 	auto seducer = scene->CreateGameObject();
	 	seducer->GetComponent<Name>()->name = "seducer";
	 	seducer->Transform()->position = vec3{ 2, 0.125, 0 };
	 	seducer->Transform()->scale = vec3{ 1.f / 4 };
	 	seducer->AddComponent<RigidBody>()->initial_velocity = vec3{ -2, 0, 0 };
	 	auto mesh_rend = seducer->AddComponent<MeshRenderer>(); 
	 	mesh_rend->mesh = Mesh::defaults[MeshType::Sphere];
	 	mesh_rend->material_instance = mat_inst;
	 	seducer->AddComponent<Collider>()->shape = sphere{ vec3{}, 1 };
	 }
	 
	 {
	 	auto seducer = scene->CreateGameObject();
	 	seducer->GetComponent<Name>()->name = "seducer";
	 	seducer->Transform()->position = vec3{ 1, 0.125, 0 };
	 	seducer->Transform()->scale = vec3{ 1.f / 4 };
	 	seducer->AddComponent<RigidBody>()->initial_velocity = vec3{ -2, 0, 0 };
	 	auto mesh_rend = seducer->AddComponent<MeshRenderer>();
	 	mesh_rend->mesh = Mesh::defaults[MeshType::Circle];
	 	mesh_rend->material_instance = mat_inst;
	 	//seducer->AddComponent<Collider>()->shape = sphere{ vec3{}, 1 };
	 }
	
	if(0)
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



    Core::GetResourceManager().Load<Prefab>("/assets/prefabs/testprefab2.idp").value()->Instantiate(*scene);
    Core::GetResourceManager().Load<Prefab>("/assets/prefabs/testprefab2.idp").value()->Instantiate(*scene);



	c->Run();
	
	auto retval = c->GetSystem<Windows>().GetReturnVal();
	c.reset();
	return retval;
}
