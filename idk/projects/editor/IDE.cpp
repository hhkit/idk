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
#include <IDE.h>

#include <filesystem>

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

// resource importing
#include <res/EasyFactory.h>
#include <loading/AssimpImporter.h>
#include <loading/GraphFactory.h>
#include <opengl/resource/OpenGLCubeMapLoader.h>
#include <opengl/resource/OpenGLTextureLoader.h>
#include <opengl/resource/OpenGLFontAtlasLoader.h>

// editor setup
#include <gfx/RenderTarget.h>
#include <editor/SceneManagement.h>
#include <editor/commands/CommandList.h>
#include <editor/windows/IGE_WindowList.h>
#include <editor/windows/IGE_ShadowMapWindow.h>

// util
#include <util/ioutils.h>

namespace fs = std::filesystem;

namespace idk
{
	IDE::IDE()
	{
	}

	void IDE::Init()
	{
        // project load
        {
            auto& proj_manager = Core::GetSystem<ProjectManager>();
            const auto recent_proj = []() -> string
            {
                fs::path recent_path = Core::GetSystem<FileSystem>().GetAppDataDir();
                recent_path /= "idk";
                recent_path /= ".recent";
                if (!fs::exists(recent_path))
                    return "";
                std::ifstream recent_file{ recent_path };
                fs::path proj = stringify(recent_file);
                if (!fs::exists(proj))
                    return "";
                return proj.string();
            }();

            if (recent_proj.empty())
            {
                const DialogOptions dialog{ "IDK Project", ProjectManager::ext };
                auto proj = Core::GetSystem<Application>().OpenFileDialog(dialog);
                while (!proj)
                    proj = Core::GetSystem<Application>().OpenFileDialog(dialog);
                proj_manager.LoadProject(*proj);
            }
            else
                proj_manager.LoadProject(recent_proj);

            fs::path recent_path = Core::GetSystem<FileSystem>().GetAppDataDir();
            recent_path /= "idk";
            if (!fs::exists(recent_path))
                fs::create_directory(recent_path);
            _editor_app_data = recent_path.string();

            recent_path /= ".recent";
            std::ofstream recent_file{ recent_path };
            recent_file << proj_manager.GetProjectFullPath();
        }

		Core::GetGameState().OnObjectDestroy<GameObject>().Listen([&](Handle<GameObject> h)
		{
			selected_gameObjects.erase(std::remove(selected_gameObjects.begin(), selected_gameObjects.end(), h), selected_gameObjects.end());
		});
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
			break;
		default:
			break;
		}

        Core::GetResourceManager().RegisterLoader<AssimpImporter>(".fbx");
        Core::GetResourceManager().RegisterLoader<AssimpImporter>(".obj");
        Core::GetResourceManager().RegisterLoader<AssimpImporter>(".md5mesh");
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
        ImGui::LoadIniSettingsFromDisk((_editor_app_data + "/imgui.ini").c_str());

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

        // font config
        ImFontConfig config;
        config.OversampleH = 5;
        config.OversampleV = 3;
        config.RasterizerMultiply = 1.5f;
        auto fontpath = fs.GetFullPath("/editor_data/fonts/SourceSansPro-Regular.ttf");
        auto fontpathbold = fs.GetFullPath("/editor_data/fonts/SourceSansPro-SemiBold.ttf");
        io.Fonts->AddFontFromFileTTF(fontpath.c_str(), 15.0f, &config); // Default
        io.Fonts->AddFontFromFileTTF(fontpath.c_str(), 13.0f, &config); // Smaller
        io.Fonts->AddFontFromFileTTF(fontpathbold.c_str(), 15.0f, &config); // Bold



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
		ADD_WINDOW(IGE_MaterialEditor);
		ADD_WINDOW(IGE_AnimatorWindow);
		ADD_WINDOW(IGE_ProfilerWindow);
		ADD_WINDOW(IGE_ProjectSettings);	
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
            {
                auto scene = h.AsHandle<Scene>();
                auto active_scene = Core::GetSystem<SceneManager>().GetActiveScene();
                if (scene != active_scene)
                {
                    if (active_scene)
                        active_scene->Unload();
                    Core::GetSystem<SceneManager>().SetActiveScene(scene);
                    scene->Load();

                    Core::GetSystem<IDE>().ClearScene();
                }
            }
        };
	}

	void IDE::LateInit()
	{
		Core::GetScheduler().SetPauseState(EditorPause);

        for (auto& elem : Core::GetSystem<FileSystem>().GetEntries("/assets", FS_FILTERS::FILE | FS_FILTERS::RECURSE_DIRS))
        {
            if (elem.GetExtension() != ".meta")
                Core::GetResourceManager().Load(elem, false);
        }

		SetupEditorScene();
		
	}

	void IDE::Shutdown()
	{
        ImGui::SaveIniSettingsToDisk((_editor_app_data + "/imgui.ini").c_str());
        Core::GetSystem<ProjectManager>().SaveConfigs();
        ige_windows.clear();
		_interface->Shutdown();
		_interface.reset();
	}

	void IDE::EditorUpdate()
	{
        if (closing)
            return;

		// scene controls
		auto& app = Core::GetSystem<Application>();
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


		//_interface->Inputs()->Update(); //Moved to SceneView.cpp
		//_interface->ImGuiFrameUpdate();
		
		
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
			camHandle->is_scene_camera = true;
			camHandle->clear = color{ 0.05f, 0.05f, 0.1f, 1.f };
			//if (Core::GetSystem<GraphicsSystem>().GetAPI() != GraphicsAPI::Vulkan)
				camHandle->clear = *Core::GetResourceManager().Load<CubeMap>("/engine_data/textures/skybox/space.png.cbm", false);

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
					finalCamPos += transform->position;
				}

			}

            finalCamPos /= static_cast<float>(selected_gameObjects.size());

			const float distanceFromObject = 10; //Needs to be dependent of spacing of objects

			const CameraControls& main_camera = Core::GetSystem<IDE>()._interface->Inputs()->main_camera;
			const Handle<Camera> currCamera = main_camera.current_camera;
			const Handle<Transform> camTransform = currCamera->GetGameObject()->GetComponent<Transform>();
			camTransform->position = finalCamPos;
			focused_vector = finalCamPos;
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
