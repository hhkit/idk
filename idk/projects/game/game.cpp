#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#include <debug/LogSystem.h>
#include <core/Core.h>
#include <vkn/VulkanWin32GraphicsSystem.h>
#include <opengl/system/OpenGLGraphicsSystem.h>
#include <win32/WindowsApplication.h>
#include <win32/XInputSystem.h>
#include <ReflectRegistration.h>
#include <editor/IDE.h>
#include <file/FileSystem.h>
#include <gfx/MeshRenderer.h>
#include <scene/SceneManager.h>
#include <test/TestComponent.h>

#include <script/ScriptSystem.h>
#include <script/MonoBehaviorEnvironment.h>

#include <serialize/text.h>

#include <gfx/CameraControls.h>

#include <test/TestSystem.h>
#include <renderdoc/renderdoc_app.h>

#include <shellapi.h>//CommandLineToArgv

#include <natvis_ids.h>

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

	using namespace idk;

	auto c = std::make_unique<Core>();
	
    auto& win = c->AddSystem<Windows>(hInstance, nCmdShow);
    c->AddSystem<win::XInputSystem>();

	GraphicsSystem* gSys = nullptr;
	auto gfx_api = HasArg(L"--vulkan", command_lines, num_args) ? GraphicsAPI::Vulkan : GraphicsAPI::OpenGL;
	switch (gfx_api)
	{
	case GraphicsAPI::Vulkan:
		gSys = &c->AddSystem<vkn::VulkanWin32GraphicsSystem>();
		break;
	case GraphicsAPI::OpenGL: 
		gSys = &c->AddSystem<ogl::Win32GraphicsSystem>();
		break;
	default:
		break;
	
	}
	gSys->is_deferred(HasArg(L"--deferred", command_lines, num_args));

	c->AddSystem<IDE>();

	c->Setup();

	//LogSingleton::Get().PipeToCout(LogPool::GAME, true);
	//Core::GetSystem<mono::ScriptSystem>().ScriptEnvironment().Execute();

	//gSys->brdf = *Core::GetResourceManager().Load<ShaderProgram>("/engine_data/shaders/brdf.frag", false);
	//gSys->convoluter = *Core::GetResourceManager().Load<ShaderProgram>("/engine_data/shaders/pbr_convolute.frag", false);

    // auto scene = RscHandle<Scene>{};
    // auto go = scene->CreateGameObject();
    // go->AddComponent<ParticleSystem>();
	
	//auto scene = RscHandle<Scene>{};
	//auto mat_inst = Core::GetResourceManager().Create<MaterialInstance>();
	//mat_inst->material = Core::GetResourceManager().Load<shadergraph::Graph>("/assets/materials/test.mat", false).value();
	//
	//auto create_anim_obj = [&](vec3 pos, PathHandle path = PathHandle{ "/assets/models/YY_model.fbx" }) {
	//	auto go = scene->CreateGameObject();

	//	go->Name(path.GetStem());
	//	go->GetComponent<Transform>()->position = pos;
	//	//go->GetComponent<Transform>()->scale /= 100.0f;

	//	auto model_resource = Core::GetResourceManager().Load(path);
	//	string model_stem{ path.GetStem() };
	//	auto animator = go->AddComponent<Animator>();
	//	for (auto handle : model_resource->GetAll<Mesh>())
	//	{
	//		auto mesh_child_go = scene->CreateGameObject();

	//		mesh_child_go->Name(handle->Name());
	//		mesh_child_go->Transform()->parent = go;

	//		auto mesh_rend = mesh_child_go->AddComponent<SkinnedMeshRenderer>();
	//		mesh_rend->mesh = handle;
	//		mesh_rend->material_instance = mat_inst;
	//	}

	//	animator->skeleton = model_resource->Get<anim::Skeleton>();
	//	Core::GetSystem<AnimationSystem>().GenerateSkeletonTree(*animator);

	//	auto animation_resource = Core::GetResourceManager().Load("/assets/models/test.fbx");
	//	animator->AddAnimation(animation_resource->Get<anim::Animation>());
	//	animation_resource = Core::GetResourceManager().Load("/assets/models/test2.fbx");
	//	animator->AddAnimation(animation_resource->Get<anim::Animation>());
	//	animation_resource = Core::GetResourceManager().Load("/assets/models/walk.fbx");
	//	animator->AddAnimation(animation_resource->Get<anim::Animation>());
	//	animation_resource = Core::GetResourceManager().Load("/assets/models/idle.fbx");
	//	animator->AddAnimation(animation_resource->Get<anim::Animation>());
	//	
	//	return go;
	//};

	//// @Joseph: Uncomment this when testing.
	//create_anim_obj(vec3{ 0,0,0 });
	
	c->Run();
	return c->GetSystem<Windows>().GetReturnVal();
}
