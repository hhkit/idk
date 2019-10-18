#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#include <core/Core.h>
#include <vkn/VulkanWin32GraphicsSystem.h>
#include <vkn/VulkanDebugRenderer.h>
#include <opengl/system/OpenGLGraphicsSystem.h>
#include <win32/WindowsApplication.h>
#include <win32/XInputSystem.h>
#include <ReflectRegistration.h>
#include <editor/IDE.h>
#include <file/FileSystem.h>
#include <gfx/MeshRenderer.h>
#include <scene/SceneManager.h>
#include <test/TestComponent.h>
#include <math/euler_angles.h>
#include <iostream>
#include <editor/loading/OpenGLCubeMapLoader.h>

#include <script/ScriptSystem.h>
#include <script/MonoBehaviorEnvironment.h>

#include <serialize/text.h>

#include <gfx/CameraControls.h>

#include <test/TestSystem.h>
#include <renderdoc/renderdoc_app.h>

#include <shellapi.h>//CommandLineToArgv

#include <natvis_ids.h>

#define USE_RENDER_DOC

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
    //c->AddSystem<win::XInputSystem>();

	GraphicsSystem* gSys = nullptr;
	auto gfx_api = HasArg(L"--vulkan", command_lines, num_args) ? GraphicsAPI::Vulkan : GraphicsAPI::OpenGL;
	switch (gfx_api)
	{
	case GraphicsAPI::Vulkan:
		gSys = &c->AddSystem<vkn::VulkanWin32GraphicsSystem>();
		win.OnScreenSizeChanged.Listen([gSys](const ivec2&) { s_cast<vkn::VulkanWin32GraphicsSystem*>(gSys)->Instance().OnResize(); });
		break;
	case GraphicsAPI::OpenGL: 
		gSys = &c->AddSystem<ogl::Win32GraphicsSystem>();
		break;
	default:
		break;
	}
	c->AddSystem<IDE>();

	c->Setup();

	Core::GetSystem<LogSystem>().PipeToCout(LogPool::GAME, true);
	//Core::GetSystem<mono::ScriptSystem>().ScriptEnvironment().Execute();

	gSys->brdf = *Core::GetResourceManager().Load<ShaderProgram>("/assets/shader/brdf.frag", false);
	gSys->convoluter = *Core::GetResourceManager().Load<ShaderProgram>("/assets/shader/pbr_convolute.frag", false);

	Core::GetResourceManager().Create<TestResource>("/assets/test/yolo.test");

	auto minecraft_texture = *Core::GetResourceManager().Load<Texture>("/assets/textures/DebugTerrain.png", false);

	auto scene = RscHandle<Scene>{};

	float divByVal = 2.f;

	// create editor camera
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
		camHandle->render_target = RscHandle<RenderTarget>{};
		camHandle->clear = color{ 0.05f, 0.05f, 0.1f, 1.f };
		if(gfx_api!=GraphicsAPI::Vulkan)
			camHandle->clear = *Core::GetResourceManager().Load<CubeMap>("/assets/textures/skybox/space.png.cbm", false);

		if (&c->GetSystem<IDE>())
		{
			Core::GetSystem<IDE>().currentCamera().current_camera = camHandle;
			divByVal = 200.f;
		}
	}
	auto mat_inst = Core::GetResourceManager().Create<MaterialInstance>();
	mat_inst->material = Core::GetResourceManager().Load<shadergraph::Graph>("/assets/materials/test.mat", false).value();
	mat_inst->uniforms["tex"] = minecraft_texture;

	
	auto create_anim_obj = [&scene, mat_inst, gfx_api, divByVal](vec3 pos, PathHandle path = PathHandle{ "/assets/models/YY_model.fbx" }) {
		auto go = scene->CreateGameObject();

		go->Name(path.GetStem());
		go->GetComponent<Transform>()->position = pos;
		//go->GetComponent<Transform>()->scale /= 100.0f;

		auto model_resource = Core::GetResourceManager().Load(path);
		string model_stem{ path.GetStem() };
		auto animator = go->AddComponent<Animator>();
		for (auto handle : model_resource->GetAll<Mesh>())
		{
			auto mesh_child_go = scene->CreateGameObject();

			mesh_child_go->Name(handle->Name());
			mesh_child_go->Transform()->parent = go;

			auto mesh_rend = mesh_child_go->AddComponent<SkinnedMeshRenderer>();
			mesh_rend->mesh = handle;
			mesh_rend->material_instance = mat_inst;
		}

		animator->SetSkeleton(model_resource->Get<anim::Skeleton>());

		// Load other animations
		PathHandle parent_dir{ path.GetParentMountPath() };
		auto entries = parent_dir.GetEntries(FS_FILTERS::FILE | FS_FILTERS::EXT, ".fbx");
		for (auto& file : entries)
		{
			string file_name{ file.GetFileName() };
			
			// Find all fbx files formatted like "model_name@anim_name.fbx"
			if (file_name.find(model_stem + "@") != string::npos)
			{
				std::cout << "Found animation file: " << file_name << "." << std::endl;
				auto animation_resource = Core::GetResourceManager().Load(file);
				for (auto& anim : animation_resource->GetAll<anim::Animation>())
				{
					std::cout << "Adding animation: " << anim->Name() << "." << std::endl;
					animator->AddAnimation(anim);
				}
				
			}
		}
		
		return go;
	};


	auto tmp_tex = minecraft_texture;
	if (gfx_api == GraphicsAPI::Vulkan)
		tmp_tex = *Core::GetResourceManager().Load<Texture>("/assets/textures/texture.dds", false);

	constexpr auto col = ivec3{ 1,0,0 };

	// @Joseph: Uncomment this when testing.
	//create_anim_obj(vec3{ 0,0,0 });
	//create_mesh_obj();	// Create just a mesh object

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
;
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
		mesh_rend->material_instance->uniforms["tex"] = *Core::GetResourceManager().Load<Texture>("/assets/textures/Grass.jpg", false);
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
				color{1,1,1}
			};
			auto light_map = Core::GetResourceManager().Create<RenderTarget>();
			auto m = light_map->GetMeta().textures[0]->GetMeta();
			m.filter_mode = FilterMode::Nearest;
			m.uv_mode = UVMode::ClampToBorder;
			light_map->GetMeta().textures[0]->SetMeta(m);
			light_comp->SetLightMap(light_map);
		}
		light->AddComponent<TestComponent>();
	}

    Core::GetResourceManager().Load<Prefab>("/assets/prefabs/testprefab2.idp", false).value()->Instantiate(*scene);
    Core::GetResourceManager().Load<Prefab>("/assets/prefabs/testprefab2.idp", false).value()->Instantiate(*scene);

	c->Run();
	return c->GetSystem<Windows>().GetReturnVal();
}
