//////////////////////////////////////////////////////////////////////////////////
//@file		IDE.cpp
//@author	Muhammad Izha B Rahim, Wei Xiang
//@param	Email : izha95\@hotmail.com
//@date		9 SEPT 2019
//@brief	

/*
Main Brain of the Game Editor

Accessible through Core::GetSystem<IDE>() [#include <IDE.h>]

*/
//////////////////////////////////////////////////////////////////////////////////


#include "pch.h"
#include <core/Scheduler.inl>
//#define HACKING_TO_THE_GATE
#include <IDE.h>

#include <file/FileSystem.h>

// dependencies
#include <imgui/imgui.h>
#include <imgui/ImGuizmo.h>

// project management and setup
#include <proj/ProjectManager.h>
#include <common/TagManager.h>
#include <PauseConfigurations.h>
#include <vkn/VulkanWin32GraphicsSystem.h>
#include <opengl/system/OpenGLGraphicsSystem.h>

#include <scene/SceneManager.h>
#include <script/ScriptSystem.h>

// resource importing
#include <res/ResourceHandle.inl>
#include <res/EasyFactory.h>
#include <loading/AssimpImporter.h>
#include <gfx/GraphFactory.h>
#include <opengl/resource/OpenGLCubeMapLoader.h>
#include <opengl/resource/OpenGLTextureLoader.h>
#include <opengl/resource/OpenGLFontAtlasLoader.h>
#include <vkn/VulkanGlslLoader.h>
#include <gfx/ShaderSnippetLoader.h>
#include <res/ResourceHandle.inl>
#include <res/ResourceManager.inl>
#include <res/ResourceHandle.inl>
#include <res/ResourceUtils.inl>

// editor setup
#include <gfx/RenderTarget.h>
#include <editor/SceneManagement.h>
#include <editor/ProjectManagement.h>
#include <editor/commands/CommandList.h>
#include <editor/windows/IGE_WindowList.h>
#include <editor/windows/IGE_ShadowMapWindow.h>

// util
#include <util/ioutils.h>
#include <ds/span.inl>
#include <ds/result.inl>
#include <serialize/text.inl>
#include <reflect/reflect.inl>
#include <core/GameObject.inl>

namespace fs = std::filesystem;

namespace idk
{
	IDE::IDE()
	{
	}

	void IDE::Init()
	{
        fs::path idk_app_data = Core::GetSystem<FileSystem>().GetAppDataDir();
        idk_app_data /= "idk";
        Core::GetSystem<FileSystem>().Mount(idk_app_data.string(), path_idk_app_data, false);

		auto tmp_path = idk_app_data / "tmp";
		if (!fs::exists(tmp_path))
			fs::create_directory(tmp_path);
		Core::GetSystem<FileSystem>().Mount(tmp_path.string(), path_tmp, false);
#ifdef HACKING_TO_THE_GATE
		Core::GetSystem<ProjectManager>().LoadProject(string{ Core::GetSystem<FileSystem>().GetExeDir() } +"/project/hydeandseek.idk");
#else
        LoadRecentProject();
#endif
		Core::GetGameState().OnObjectDestroy<GameObject>().Listen([&](Handle<GameObject> h)
		{
			selected_gameObjects.erase(std::remove(selected_gameObjects.begin(), selected_gameObjects.end(), h), selected_gameObjects.end());
		});
		Core::GetResourceManager().RegisterLoader<ShaderSnippetLoader>(".glsl");
		switch (Core::GetSystem<GraphicsSystem>().GetAPI())
		{
		case GraphicsAPI::OpenGL:
			_interface = std::make_unique<edt::OI_Interface>(&Core::GetSystem<ogl::Win32GraphicsSystem>().Instance());
            Core::GetResourceManager().RegisterLoader<OpenGLCubeMapLoader>(".cbm");
            Core::GetResourceManager().RegisterLoader<OpenGLTextureLoader>(".png");
            Core::GetResourceManager().RegisterLoader<OpenGLTextureLoader>(".tga");
            Core::GetResourceManager().RegisterLoader<OpenGLTextureLoader>(".jpg");
            Core::GetResourceManager().RegisterLoader<OpenGLTextureLoader>(".jpeg");
            Core::GetResourceManager().RegisterLoader<OpenGLTextureLoader>(".dds");
			Core::GetResourceManager().RegisterLoader<OpenGLFontAtlasLoader>(".ttf");
			break;
		case GraphicsAPI::Vulkan:
			_interface = std::make_unique<edt::VI_Interface>(&Core::GetSystem<vkn::VulkanWin32GraphicsSystem>().Instance());

			Core::GetResourceManager().RegisterLoader<vkn::VulkanGlslLoader>(".vert");
			Core::GetResourceManager().RegisterLoader<vkn::VulkanGlslLoader>(".frag");
			Core::GetResourceManager().RegisterLoader<vkn::VulkanGlslLoader>(".geom");
			Core::GetResourceManager().RegisterLoader<vkn::VulkanGlslLoader>(".tesc");
			Core::GetResourceManager().RegisterLoader<vkn::VulkanGlslLoader>(".tese");
			Core::GetResourceManager().RegisterLoader<vkn::VulkanGlslLoader>(".comp");
			break;
		default:
			break;
		}
		//Core::GetResourceManager().RegisterLoader<AssimpImporter>(".fbx");
        //Core::GetResourceManager().RegisterLoader<AssimpImporter>(".obj");
        //Core::GetResourceManager().RegisterLoader<AssimpImporter>(".md5mesh");
        Core::GetResourceManager().RegisterLoader<GraphLoader>(shadergraph::Graph::ext);

		Core::GetResourceManager().RegisterFactory<GraphFactory>();
        Core::GetSystem<Application>().OnClosed.Listen([&]() { closing = true; });

        auto& fs = Core::GetSystem<FileSystem>();
        fs.Mount(string{ fs.GetExeDir() } + "/editor_data", "/editor_data", false);
        if (shadergraph::NodeTemplate::GetTable().empty())
            shadergraph::NodeTemplate::LoadTable("/editor_data/nodes");


		//ImGui Initializations
		_interface->Init();
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags = ImGuiConfigFlags_DockingEnable;
        io.IniFilename = NULL;
        ImGui::LoadIniSettingsFromDisk(Core::GetSystem<FileSystem>().GetFullPath("/idk/imgui.ini").c_str());

        //Imgui Style
        auto& style = ImGui::GetStyle();
        style.FramePadding = ImVec2(4.0f, 0);
        style.ItemSpacing = ImVec2(6.0f, 3.0f);
        style.ItemSpacing = ImVec2(6.0f, 3.0f);
        style.WindowRounding = 0;
        style.TabRounding = 0;
        style.IndentSpacing = 14.0f;
        style.ScrollbarRounding = 0;
        style.GrabRounding = 0;
        style.ChildRounding = 0;
        style.PopupRounding = 0;
        style.FrameRounding = 1.0f;
        style.CurveTessellationTol = 0.5f;

		ApplyDefaultColors();

        // font config
        ImFontConfig config;
        config.OversampleH = 5;
        config.OversampleV = 3;
        config.RasterizerMultiply = 1.5f;
        auto fontpath = fs.GetFullPath("/editor_data/fonts/SourceSansPro-Regular.ttf");
        auto fontpathbold = fs.GetFullPath("/editor_data/fonts/SourceSansPro-SemiBold.ttf");
        auto iconfontpath = fs.GetFullPath("/editor_data/fonts/" FONT_ICON_FILE_NAME_FAR);
        auto iconfontpath2 = fs.GetFullPath("/editor_data/fonts/" FONT_ICON_FILE_NAME_FAS);
        io.Fonts->AddFontFromFileTTF(fontpath.c_str(), 15.0f, &config); // Default

        // merge in icons from MDI
        static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
        ImFontConfig icons_config; icons_config.MergeMode = true; icons_config.PixelSnapH = true; icons_config.GlyphMinAdvanceX = 14.0f; icons_config.GlyphOffset.y = 1.0f;
        io.Fonts->AddFontFromFileTTF(iconfontpath.c_str(), 14.0f, &icons_config, icons_ranges);
        io.Fonts->AddFontFromFileTTF(iconfontpath2.c_str(), 14.0f, &icons_config, icons_ranges);

        // smaller and bold style of default font
        io.Fonts->AddFontFromFileTTF(fontpath.c_str(), 13.0f, &config); // Smaller
        io.Fonts->AddFontFromFileTTF(iconfontpath.c_str(), 12.0f, &icons_config, icons_ranges);
        io.Fonts->AddFontFromFileTTF(iconfontpath2.c_str(), 12.0f, &icons_config, icons_ranges);

        io.Fonts->AddFontFromFileTTF(fontpathbold.c_str(), 15.0f, &config); // Bold
        io.Fonts->AddFontFromFileTTF(iconfontpath.c_str(), 14.0f, &icons_config, icons_ranges);
        io.Fonts->AddFontFromFileTTF(iconfontpath2.c_str(), 14.0f, &icons_config, icons_ranges);

        io.Fonts->Build();


		//Window Initializations
		ige_main_window = std::make_unique<IGE_MainWindow>();

#define ADD_WINDOW(type) windows_by_type.emplace(reflect::typehash<type>(), ige_windows.emplace_back(std::make_unique<type>()).get());
		ADD_WINDOW(IGE_GameView);
		ADD_WINDOW(IGE_SceneView);
		ADD_WINDOW(IGE_ShadowMapWindow);
		ADD_WINDOW(IGE_Console);
		ADD_WINDOW(IGE_ProgrammerConsole);
		ADD_WINDOW(IGE_ProjectWindow);
		ADD_WINDOW(IGE_HierarchyWindow);
		ADD_WINDOW(IGE_InspectorWindow);
		ADD_WINDOW(IGE_LightLister);
		ADD_WINDOW(IGE_MaterialEditor);
		ADD_WINDOW(IGE_AnimatorWindow);
		ADD_WINDOW(IGE_ProfilerWindow);
		ADD_WINDOW(IGE_ProjectSettings);	
		ADD_WINDOW(IGE_HelpWindow);	
#undef ADD_WINDOW

		ige_main_window->Initialize();
		for (auto& i : ige_windows) {
			i->Initialize();
		}

		Core::GetSystem<SceneManager>().OnSceneChange +=
			[](RscHandle<Scene> active_scene)
		{
			if (auto path = Core::GetResourceManager().GetPath(active_scene))
				Core::GetSystem<Application>().SetTitle(string{ "idk: " } +string{ *path });
			else
				Core::GetSystem<Application>().SetTitle(string{ " idk " });
		};

		Core::GetSystem<ProjectManager>().OnProjectSaved +=
			[]()
		{
			auto active_scene = Core::GetSystem<SceneManager>().GetActiveScene();
			if (auto path = Core::GetResourceManager().GetPath(active_scene))
				Core::GetSystem<Application>().SetTitle(string{ "idk: " } +string{ *path });
			else
				Core::GetSystem<Application>().SetTitle(string{ " idk " });

		};

        FindWindow<IGE_ProjectWindow>()->OnAssetDoubleClicked += [](GenericResourceHandle h)
        {
            if (h.index() == BaseResourceID<Scene>)
                OpenScene(h.AsHandle<Scene>());
        };
	}

	void IDE::LateInit()
	{
		Core::GetScheduler().SetPauseState(EditorPause);

        for (auto& elem : Core::GetSystem<FileSystem>().GetEntries("/assets", FS_FILTERS::FILE | FS_FILTERS::RECURSE_DIRS))
        {
			Core::GetResourceManager().Load(elem);
        }
		Core::GetSystem<Application>().WaitForChildren();

		SetupEditorScene();
		Core::GetSystem<mono::ScriptSystem>().run_scripts = false;


        { // try load recent scene / camera
            auto last_scene = reg_scene.get("scene");
            if (last_scene.size())
            {
                RscHandle<Scene> h{ Guid(last_scene) };
                if (!h)
                {
                    NewScene();
                    return;
                }

                OpenScene(h);

                auto cam = reg_scene.get("camera");
                if (cam.size())
                {
                    auto& t = *currentCamera().current_camera->GetGameObject()->Transform();
                    auto res = parse_text<Transform>(cam);
                    if (res)
                    {
                        auto last_t = *res;
                        t.position = last_t.position;
                        t.rotation = last_t.rotation;
                        t.scale = last_t.scale;
                    }
                }
            }
            else
                NewScene(); 
        }
	}

    void IDE::EarlyShutdown()
    {
        reg_scene.set("camera", serialize_text(*currentCamera().current_camera->GetGameObject()->Transform()));

        ImGui::SaveIniSettingsToDisk(Core::GetSystem<FileSystem>().GetFullPath("/idk/imgui.ini").c_str());
        Core::GetSystem<ProjectManager>().SaveConfigs();
        ige_windows.clear();
        _interface->Shutdown();
        _interface.reset();
    }

	void IDE::Shutdown()
	{
	}

	void IDE::EditorUpdate()
	{
        if (closing)
            return;

        auto& app = Core::GetSystem<Application>();

        static auto fullscreen = true;
        if (ImGui::IsKeyPressed(static_cast<int>(Key::F)) && ImGui::IsKeyDown(static_cast<int>(Key::Control)))
            app.SetFullscreen(fullscreen = !fullscreen);

		// scene controls
		if (!game_running)
			if (app.GetKey(Key::Control))
			{
				if(app.GetKeyDown(Key::S))
				{
					if (app.GetKey(Key::Shift))
						SaveSceneAs();
					else
						SaveScene();
				}
				if (app.GetKeyDown(Key::N))
					NewScene();
				if (app.GetKeyDown(Key::O))
					OpenScene();
			}

		_interface->ImGuiFrameBegin();
		ImGuizmo::BeginFrame();

		{ //Erase the nullptrs.
			vector<size_t> null_objects;
			size_t index = 0;
			for (const auto go : selected_gameObjects)
			{
				if (!go)
					null_objects.emplace_back(index);
				++index;
			}
			std::reverse(null_objects.begin(), null_objects.end());
			for (auto i : null_objects)
				selected_gameObjects.erase(selected_gameObjects.begin() + i); //erase in reverse order to ensure that the indices don't change.
		}
        for (const auto go : selected_gameObjects)
        {
			if (const auto col = go->GetComponent<Collider>())
				Core::GetSystem<PhysicsSystem>().DrawCollider(*col);
        }
		

		ige_main_window->DrawWindow();

		for (auto& i : ige_windows) {
			if (flag_skip_render) {
				flag_skip_render = false;
				break;
			}
			i->DrawWindow();
		}

		if (bool_demo_window)
			ImGui::ShowDemoWindow(&bool_demo_window);

		_interface->ImGuiFrameEnd();

		command_controller.FlushCommands();
	}

	void IDE::EditorDraw()
	{
		// call imgui draw,
		_interface->ImGuiFrameRender();
	}
	CameraControls& IDE::currentCamera()
	{
		// TODO: insert return statement here
		return _interface->Inputs()->main_camera;
	}

	void IDE::ClearScene()
	{
		// clear removed tags
		const auto num_tags = Core::GetSystem<TagManager>().GetNumOfTags();
		for (const auto& c : GameState::GetGameState().GetObjectsOfType<Tag>())
		{
			if (c.index > num_tags)
				c.GetGameObject()->RemoveComponent(c.GetHandle());
		}

		//Clear IDE values
		Core::GetSystem<IDE>().flag_skip_render = true;
		Core::GetSystem<IDE>().command_controller.ClearUndoRedoStack();
		Core::GetSystem<IDE>().selected_gameObjects.clear();
		Core::GetSystem<IDE>().selected_matrix.clear();
	}

	string_view IDE::GetTmpSceneMountPath() const
	{
		return "/tmp/tmp_scene.ids";
	}

	void IDE::ApplyDefaultColors()
	{
		auto& style = ImGui::GetStyle();
		auto* colors = style.Colors;
		ImGui::StyleColorsDark();

		colors[ImGuiCol_CheckMark] = colors[ImGuiCol_Text];

		// grays
		colors[ImGuiCol_WindowBg] =
			ImColor(29, 34, 41).Value;
		colors[ImGuiCol_PopupBg] =
			colors[ImGuiCol_ScrollbarBg] =
			ImColor(43, 49, 56, 240).Value;
		colors[ImGuiCol_Border] =
			colors[ImGuiCol_Separator] =
			colors[ImGuiCol_ScrollbarGrab] =
			ImColor(63, 70, 77).Value;
		//ImColor(106, 118, 129).Value;

		colors[ImGuiCol_MenuBarBg] =
			ImColor(36, 58, 74).Value;
		colors[ImGuiCol_ScrollbarBg].w = 0.5f;

		// main accent - 2
		colors[ImGuiCol_TitleBg] =
			ImColor(5, 30, 51).Value;

		// main accent - 1
		colors[ImGuiCol_TitleBgActive] =
			colors[ImGuiCol_TabUnfocused] =
			ImColor(11, 54, 79).Value;

		// main accent
		colors[ImGuiCol_Tab] =
			colors[ImGuiCol_TabUnfocusedActive] =
			colors[ImGuiCol_FrameBg] =
			colors[ImGuiCol_Button] =
			colors[ImGuiCol_Header] =
			colors[ImGuiCol_SeparatorHovered] =
			colors[ImGuiCol_ScrollbarGrabHovered] =
			ImColor(23, 75, 111).Value;

		// main accent + 1
		colors[ImGuiCol_TabHovered] =
			colors[ImGuiCol_TabActive] =
			colors[ImGuiCol_ButtonHovered] =
			colors[ImGuiCol_FrameBgHovered] =
			colors[ImGuiCol_HeaderHovered] =
			colors[ImGuiCol_SeparatorActive] =
			colors[ImGuiCol_ScrollbarGrabActive] =
			ImColor(46, 115, 143).Value;

		// main accent + 2
		colors[ImGuiCol_TextSelectedBg] =
			colors[ImGuiCol_ButtonActive] =
			colors[ImGuiCol_FrameBgActive] =
			colors[ImGuiCol_HeaderActive] =
			ImColor(65, 153, 163).Value;

		// complement accent
		colors[ImGuiCol_PlotLinesHovered] =
			ImColor(222, 116, 35).Value;
		//style.Colors[ImGuiCol_PlotLinesHovered]
		// ImColor(

	}

	void IDE::RefreshSelectedMatrix()
	{
		//Refresh the new matrix values
		selected_matrix.clear();
		for (const auto& i : selected_gameObjects) {
			selected_matrix.push_back(i->GetComponent<Transform>()->GlobalMatrix());
		}
	}

	void IDE::SetupEditorScene()
	{
		// create editor view
		editor_view = Core::GetResourceManager().Create<RenderTarget>();
		auto sz = editor_view->Size();
		editor_view->Size(Core::GetSystem<Application>().GetScreenSize());
		//this->FindWindow<IGE_Console>()->PushMessage(std::to_string(sz.x) + "," + std::to_string(sz.y));
		editor_view->Name("Editor View");
		//editor_view->Size();
		// create editor camera
		RscHandle<Scene> scene{};
		{
			auto camera = scene->CreateGameObject();
			Handle<Camera> camHandle = camera->AddComponent<Camera>();
			camera->GetComponent<Name>()->name = "Editor Camera";
			camera->Transform()->position = vec3{ 0, 0, 5 };
			camHandle->far_plane = 100.f;
			camHandle->render_target = editor_view;
			camHandle->clear = color{ 0.05f, 0.05f, 0.1f, 1.f };
			//if (Core::GetSystem<GraphicsSystem>().GetAPI() != GraphicsAPI::Vulkan)
				camHandle->clear = *Core::GetResourceManager().Load<CubeMap>("/engine_data/textures/skybox/VictorianStreet.png.cbm", false);

			Core::GetSystem<IDE>().currentCamera().current_camera = camHandle;
		}
	}

	void IDE::FocusOnSelectedGameObjects()
	{
		if (selected_gameObjects.size()) {
			vec3 finalCamPos{};
			for (const auto& i : selected_gameObjects) {

				const auto transform = i->GetComponent<Transform>();
				if (transform) {
					finalCamPos += transform->GlobalPosition();
				}

			}

            finalCamPos /= static_cast<float>(selected_gameObjects.size());

			/*
			Needs to find how much space the object pixels takes on screen, then this distance is based on that.
			If single object, object pixels should take 50% of screen.
			If multiple objects, the ends of each object pixels on screen plus some padding should be used as anchor points to determine distance.
			But this is lazy method. 20.
			*/
			const float distanceFromObject = 20; 

			const CameraControls& main_camera = Core::GetSystem<IDE>()._interface->Inputs()->main_camera;
			const Handle<Camera> currCamera = main_camera.current_camera;
			const Handle<Transform> camTransform = currCamera->GetGameObject()->GetComponent<Transform>();
			camTransform->position = finalCamPos;
			if (auto* sceneViewPtr = FindWindow<IGE_SceneView>())
				sceneViewPtr->focused_vector = finalCamPos;
			scroll_multiplier = default_scroll_multiplier;
			camTransform->position -= camTransform->Forward() * distanceFromObject;
		}
	}

	void IDE::IncreaseScrollPower()
	{
		scroll_multiplier += scroll_additive;
		scroll_multiplier = scroll_multiplier > scroll_max ? scroll_max : scroll_multiplier;

	}

	void IDE::DecreaseScrollPower()
	{
		scroll_multiplier -= scroll_subtractive;
		scroll_multiplier = scroll_multiplier < scroll_min ? scroll_min : scroll_multiplier;
	}

	void IDE::RecursiveCollectObjects(Handle<GameObject> i, vector<RecursiveObjects>& vector_ref)
	{

		RecursiveObjects newObject{};
		newObject.original_handle = i;
		//Copy all components from this gameobject
		for (auto& c : i->GetComponents())
			newObject.vector_of_components.emplace_back((*c).copy());

		SceneManager& sceneManager = Core::GetSystem<SceneManager>();
		SceneManager::SceneGraph* children = sceneManager.FetchSceneGraphFor(i);

		if (children) {

			//If there is no children, it will stop
			children->visit([&](const Handle<GameObject>& handle, int depth) -> bool { //Recurse through one level only
				(void)depth;

				//Skip parent
				if (handle == i)
					return true;

				RecursiveCollectObjects(handle, newObject.children); //Depth first recursive

				return false;

				});
		}
		vector_ref.push_back(newObject);
	}



}
