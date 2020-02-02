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
        LoadRecentProject();

		switch (Core::GetSystem<GraphicsSystem>().GetAPI())
		{
		case GraphicsAPI::OpenGL:
			_interface = std::make_unique<opengl_imgui_interface>(&Core::GetSystem<ogl::Win32GraphicsSystem>().Instance());
            Core::GetResourceManager().RegisterLoader<OpenGLCubeMapLoader>(".cbm");
            Core::GetResourceManager().RegisterLoader<OpenGLTextureLoader>(".png");
            Core::GetResourceManager().RegisterLoader<OpenGLTextureLoader>(".tga");
            Core::GetResourceManager().RegisterLoader<OpenGLTextureLoader>(".jpg");
            Core::GetResourceManager().RegisterLoader<OpenGLTextureLoader>(".jpeg");
            Core::GetResourceManager().RegisterLoader<OpenGLTextureLoader>(".dds");
			Core::GetResourceManager().RegisterLoader<OpenGLFontAtlasLoader>(".ttf");
			break;
		case GraphicsAPI::Vulkan:
			_interface = std::make_unique<vulkan_imgui_interface>(&Core::GetSystem<vkn::VulkanWin32GraphicsSystem>().Instance());

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
		Core::GetResourceManager().RegisterLoader<ShaderSnippetLoader>(".glsl");
        Core::GetResourceManager().RegisterLoader<GraphLoader>(shadergraph::Graph::ext);

		Core::GetResourceManager().RegisterFactory<GraphFactory>();


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

		ApplyDefaultStyle();
		ApplyDefaultColors();
		LoadEditorFonts();

		//Window Initializations
#define ADD_WINDOW(type) windows_by_type.emplace(reflect::typehash<type>(), ige_windows.emplace_back(std::make_unique<type>()).get());
		ADD_WINDOW(IGE_MainWindow);
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

		for (auto& i : ige_windows)
			i->Initialize();

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

		Core::GetSystem<Application>().OnClosed.Listen([&]() { closing = true; });

		//Core::GetGameState().OnObjectDestroy<GameObject>().Listen([&](Handle<GameObject> h)
		//{
		//	selected_gameObjects.erase(std::remove(selected_gameObjects.begin(), selected_gameObjects.end(), h), selected_gameObjects.end());
		//});

	}

	void IDE::LateInit()
	{
		Core::GetScheduler().SetPauseState(EditorPause);
		Core::GetResourceManager().LoadPaths(Core::GetSystem<FileSystem>().GetEntries("/assets", FS_FILTERS::FILE | FS_FILTERS::RECURSE_DIRS));
		Core::GetSystem<Application>().WaitForChildren();

		SetupEditorScene();
		Core::GetSystem<mono::ScriptSystem>().run_scripts = false;
		GetEditorRenderTarget()->render_debug = true;

		Core::GetSystem<Application>().OnFocusGain += [this]()
		{
			if (game_running)
				return;
			if (scripts_changed)
			{
				Core::GetSystem<mono::ScriptSystem>().CompileGameScripts();
				HotReloadDLL();
				scripts_changed = false;
			}
		};

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
                    auto& t = *_camera.current_camera->GetGameObject()->Transform();
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
        reg_scene.set("camera", serialize_text(*_camera.current_camera->GetGameObject()->Transform()));

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
        if (ImGui::IsKeyPressed(static_cast<int>(Key::F)) && ImGui::GetIO().KeyCtrl)
            app.SetFullscreen(fullscreen = !fullscreen);

		// check files
		for (auto elem : Core::GetSystem<FileSystem>().QueryFileChangesByChange(FS_CHANGE_STATUS::WRITTEN))
		{
			if (elem.GetExtension() == ".cs")
			{
				scripts_changed = true;
				break;
			}
		}

		// scene controls
		if (!game_running && ImGui::GetIO().KeyCtrl)
		{
			if (ImGui::IsKeyPressed(static_cast<int>(Key::S)))
			{
				if (ImGui::GetIO().KeyShift)
					SaveSceneAs();
				else
					SaveScene();
			}
			if (ImGui::IsKeyPressed(static_cast<int>(Key::N)))
				NewScene();
			if (ImGui::IsKeyPressed(static_cast<int>(Key::O)))
				OpenScene();
		}
		PollShortcutInputs();

		_interface->ImGuiFrameBegin();
		ImGuizmo::BeginFrame();

		//{ //Erase the nullptrs.
		//	vector<size_t> null_objects;
		//	size_t index = 0;
		//	for (const auto go : selected_gameObjects)
		//	{
		//		if (!go)
		//			null_objects.emplace_back(index);
		//		++index;
		//	}
		//	std::reverse(null_objects.begin(), null_objects.end());
		//	for (auto i : null_objects)
		//		selected_gameObjects.erase(selected_gameObjects.begin() + i); //erase in reverse order to ensure that the indices don't change.
		//}
        for (const auto go : _selected_objects.game_objects)
        {
			if (go)
			{
				if (const auto col = go->GetComponent<Collider>())
					Core::GetSystem<PhysicsSystem>().DrawCollider(*col);
			}
        }
		
		for (auto& i : ige_windows)
			i->DrawWindow();

		if (bool_demo_window)
			ImGui::ShowDemoWindow(&bool_demo_window);

		_interface->ImGuiFrameEnd();
	}

	void IDE::EditorDraw()
	{
		// call imgui draw,
		_interface->ImGuiFrameRender();
	}

	void IDE::PollShortcutInputs()
	{
		if (ImGui::IsAnyItemActive()) //Do not do any shortcuts when inputs are active! EG: Editing texts!
			return;
		if (ImGui::IsAnyMouseDown()) //Disable shortcut whenever mouse is pressed
			return;

		//CTRL + Z
		if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Z)) && ImGui::GetIO().KeyCtrl) {
			command_controller.UndoCommand();
		}

		//CTRL + Y
		if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Y)) && ImGui::GetIO().KeyCtrl) {
			command_controller.RedoCommand();
		}

		//CTRL + C
		if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_C)) && ImGui::GetIO().KeyCtrl) 
		{
			Copy();
		}

		//CTRL + V
		if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_V)) && ImGui::GetIO().KeyCtrl) 
		{
			Paste();
		}


		//Q = Move
		if (ImGui::IsKeyPressed('Q')) {
			gizmo_operation = GizmoOperation::Null;
		}
		//W = Translate
		else if (ImGui::IsKeyPressed('W')) {
			gizmo_operation = GizmoOperation::Translate;
		}
		//E = Rotate
		else if (ImGui::IsKeyPressed('E')) {
			gizmo_operation = GizmoOperation::Rotate;
		}
		//R = Scale
		else if (ImGui::IsKeyPressed('R')) {
			gizmo_operation = GizmoOperation::Scale;
		}

		//DEL = Delete
		else if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Delete)))
		{
			DeleteSelectedGameObjects();
		}

		//F = Focus on GameObject
		else if (ImGui::IsKeyPressed('F'))
		{
			FocusOnSelectedGameObjects();
		}
	}

	const ObjectSelection& IDE::GetSelectedObjects()
	{
		return _selected_objects;
	}
	void IDE::SelectGameObject(Handle<GameObject> handle, bool multiselect, bool force)
    {
		if (!handle)
			return;
		if (!multiselect)
		{
			ObjectSelection sel;
			sel.game_objects.push_back(handle);
			if (force || sel != _selected_objects)
				command_controller.ExecuteCommand(COMMAND(CMD_SelectObject, sel));
		}
		else
		{
			auto copy = _selected_objects;
			copy.game_objects.push_back(handle);
			command_controller.ExecuteCommand(COMMAND(CMD_SelectObject, copy));
		}
    }
	void IDE::SelectAsset(GenericResourceHandle handle, bool multiselect, bool force)
	{
		if (!handle.visit([](auto h) { return bool(h); }))
			return;
		if (!multiselect)
		{
			ObjectSelection sel;
			sel.assets.push_back(handle);
			if (force || sel != _selected_objects)
				command_controller.ExecuteCommand(COMMAND(CMD_SelectObject, sel));
		}
		else
		{
			auto copy = _selected_objects;
			copy.assets.push_back(handle);
			command_controller.ExecuteCommand(COMMAND(CMD_SelectObject, copy));
		}
	}
	void IDE::SetSelection(ObjectSelection selection, bool force)
	{
		if (force || selection != _selected_objects)
			command_controller.ExecuteCommand(COMMAND(CMD_SelectObject, selection));
	}
	void IDE::Unselect(bool force)
	{
		if (force || !_selected_objects.empty())
			command_controller.ExecuteCommand(COMMAND(CMD_SelectObject, ObjectSelection{}));
	}

	void IDE::CreateGameObject(Handle<GameObject> parent, string name, vector<string> initial_components)
	{
		auto* cmd = command_controller.ExecuteCommand(
			COMMAND(CMD_CreateGameObject, parent, std::move(name), std::move(initial_components)));
		SelectGameObject(cmd->GetGameObject(), false, true);
		command_controller.ExecuteCommand(COMMAND(CMD_CollateCommands, 2));
		FindWindow<IGE_HierarchyWindow>()->ScrollToSelectedInHierarchy(cmd->GetGameObject());
	}

    void IDE::DeleteSelectedGameObjects()
    {
		if (GetSelectedObjects().game_objects.empty())
			return;

		int execute_counter = 0;

		for (auto h : GetSelectedObjects().game_objects)
		{
			if (h)
			{
				command_controller.ExecuteCommand(COMMAND(CMD_DeleteGameObject, h));
				++execute_counter;
			}
		}

		Unselect(true); ++execute_counter;
		command_controller.ExecuteCommand(COMMAND(CMD_CollateCommands, execute_counter));
    }

	void IDE::Copy()
	{
		_copied_game_objects.clear();
		for (auto h : GetSelectedObjects().game_objects)
		{
			vector<RecursiveObjects> new_obj{};
			RecursiveCollectObjects(h, new_obj);
			_copied_game_objects.push_back(std::move(new_obj));
		}
	}

	void IDE::Paste()
	{
		int execute_counter = 0;
		ObjectSelection sel;

		for (auto v : _copied_game_objects)
		{
			if (v.size() && v[0].original_handle)
			{
				auto* cmd = command_controller.ExecuteCommand(COMMAND(CMD_CreateGameObject, v));
				sel.game_objects.push_back(cmd->GetGameObject());
				++execute_counter;
			}
		}

		if (execute_counter)
		{
			SetSelection(sel);
			++execute_counter;
		}

		command_controller.ExecuteCommand(COMMAND(CMD_CollateCommands, execute_counter));
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
		Core::GetSystem<IDE>().command_controller.ClearUndoRedoStack();
		Core::GetSystem<IDE>()._selected_objects.game_objects.clear();
		Core::GetSystem<IDE>()._selected_objects.assets.clear();
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
				camHandle->clear = *Core::GetResourceManager().Load<CubeMap>("/engine_data/textures/skybox/space.png.cbm", false);

			_camera.current_camera = camHandle;
		}
	}

	void IDE::FocusOnSelectedGameObjects()
	{
		if (_selected_objects.game_objects.size()) {
			vec3 finalCamPos{};
			for (const auto& i : _selected_objects.game_objects) {

				const auto transform = i->GetComponent<Transform>();
				if (transform) {
					finalCamPos += transform->GlobalPosition();
				}

			}

            finalCamPos /= static_cast<float>(_selected_objects.game_objects.size());

			/*
			Needs to find how much space the object pixels takes on screen, then this distance is based on that.
			If single object, object pixels should take 50% of screen.
			If multiple objects, the ends of each object pixels on screen plus some padding should be used as anchor points to determine distance.
			But this is lazy method. 20.
			*/
			const float distanceFromObject = 20; 

			const CameraControls& main_camera = Core::GetSystem<IDE>()._camera;
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
