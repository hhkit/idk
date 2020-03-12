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
#include <scene/SceneGraph.inl>
#include <script/ScriptSystem.h>
#include <audio/AudioSystem.h>
#include <network/NetworkSystem.h>

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
#include <editor/windows/IGE_GfxDebugWindow.h>

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

		_editor_id = serialize_text(std::chrono::high_resolution_clock::now().time_since_epoch().count());
		auto tmp_path = idk_app_data / ("tmp" + _editor_id).sv();
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
#define ADD_WINDOW(type) _windows_by_type.emplace(reflect::typehash<type>(), _ige_windows.emplace_back(std::make_unique<type>()).get());
		ADD_WINDOW(IGE_MainWindow);
		ADD_WINDOW(IGE_GameView);
		ADD_WINDOW(IGE_SceneView);
		ADD_WINDOW(IGE_ShadowMapWindow);
		ADD_WINDOW(IGE_GfxDebugWindow);
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
		ADD_WINDOW(IGE_NetworkSettings);
		ADD_WINDOW(IGE_HelpWindow);	
#undef ADD_WINDOW

		for (auto& i : _ige_windows)
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

		Core::GetSystem<Application>().OnClosed.Listen([&]() { _closing = true; });

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

		auto& fs = Core::GetSystem<FileSystem>();
		fs.Remount("/build", true);
		for (auto& elem : Core::GetSystem<FileSystem>().GetEntries("/build", FS_FILTERS::ALL | FS_FILTERS::FILE))
		{
			if (elem.GetMountPath().starts_with("/build") && elem.IsFile())
				Core::GetResourceManager().LoadCompiledAsset(elem);
		}

		
		SetupEditorScene();
		Core::GetSystem<mono::ScriptSystem>().run_scripts = false;
		GetEditorRenderTarget()->render_debug = true;

		Core::GetSystem<Application>().OnFocusGain += [this]()
		{
			if (_game_running)
				return;
			if (_scripts_changed)
			{
				Core::GetSystem<mono::ScriptSystem>().CompileGameScripts();
				HotReloadDLL();
				_scripts_changed = false;
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
                    auto& t = *_camera->GetGameObject()->Transform();
                    auto res = parse_text<Transform>(cam);
                    if (res)
                    {
                        auto last_t = *res;
                        t.position = last_t.position;
                        t.rotation = last_t.rotation;
                        t.scale = last_t.scale;
						FindWindow<IGE_SceneView>()->SetOrbitPoint(t.position + t.Forward() * 20.0f);
                    }
                }
            }
            else
                NewScene(); 
        }
	}

    void IDE::EarlyShutdown()
    {
        reg_scene.set("camera", serialize_text(*_camera->GetGameObject()->Transform()));

        ImGui::SaveIniSettingsToDisk(Core::GetSystem<FileSystem>().GetFullPath("/idk/imgui.ini").c_str());
		fs::remove_all(Core::GetSystem<FileSystem>().GetFullPath(path_tmp).sv());
        Core::GetSystem<ProjectManager>().SaveConfigs();
        _ige_windows.clear();
        _interface->Shutdown();
        _interface.reset();
    }

	void IDE::Shutdown()
	{
	}

	void IDE::EditorUpdate()
	{
        if (_closing)
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
				_scripts_changed = true;
				break;
			}
		}

		// scene controls
		if (!_game_running && ImGui::GetIO().KeyCtrl)
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

        for (const auto go : _selected_objects.game_objects)
        {
			if (!go)
				break;

			Core::GetSystem<SceneManager>().FetchSceneGraphFor(go).Visit([](Handle<GameObject> h, int)
			{
				for(const auto col : h->GetComponents<Collider>())
					Core::GetSystem<PhysicsSystem>().DrawCollider(*col);
			});
        }
		
		if (_maximized_window)
		{
			_ige_windows[0]->DrawWindow();
			for (auto& i : _ige_windows)
			{
				if (i.get() == _maximized_window)
					i->DrawWindow();
			}
		}
		else
		{
			for (auto& i : _ige_windows)
				i->DrawWindow();
		}

		if (_show_demo_window)
			ImGui::ShowDemoWindow(&_show_demo_window);

		_interface->ImGuiFrameEnd();
	}

	void IDE::EditorDraw()
	{
		// call imgui draw,
		_interface->ImGuiFrameRender();
	}

	Handle<Camera> IDE::GetEditorCamera()
	{
		return _camera;
	}

	void IDE::PollShortcutInputs()
	{
		if (ImGui::IsAnyItemActive()) //Do not do any shortcuts when inputs are active! EG: Editing texts!
			return;
		if (ImGui::IsAnyMouseDown()) //Disable shortcut whenever mouse is pressed
			return;

		if (!ImGui::GetIO().KeyCtrl)
		{
			if (ImGui::IsKeyPressed('Q'))
				gizmo_operation = GizmoOperation::Null;
			else if (ImGui::IsKeyPressed('W'))
				gizmo_operation = GizmoOperation::Translate;
			else if (ImGui::IsKeyPressed('E'))
				gizmo_operation = GizmoOperation::Rotate;
			else if (ImGui::IsKeyPressed('R'))
				gizmo_operation = GizmoOperation::Scale;
			else if (ImGui::IsKeyPressed('F'))
				FocusOnSelectedGameObjects();
		}
		else
		{
			if (ImGui::IsKeyPressed('Z'))
				_command_controller.UndoCommand();
			else if (ImGui::IsKeyPressed('Y'))
				_command_controller.RedoCommand();
			else if (ImGui::IsKeyPressed('C'))
				Copy();
			else if (ImGui::IsKeyPressed('V'))
				Paste();
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
				ExecuteCommand<CMD_SelectObject>(sel);
		}
		else
		{
			auto copy = _selected_objects;
			copy.game_objects.push_back(handle);
			ExecuteCommand<CMD_SelectObject>(copy);
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
				ExecuteCommand<CMD_SelectObject>(sel);
		}
		else
		{
			auto copy = _selected_objects;
			copy.assets.push_back(handle);
			ExecuteCommand<CMD_SelectObject>(copy);
		}
	}
	void IDE::SetSelection(ObjectSelection selection, bool force)
	{
		if (force || selection != _selected_objects)
			ExecuteCommand<CMD_SelectObject>(selection);
	}
	void IDE::Unselect(bool force)
	{
		if (force || !_selected_objects.empty())
			ExecuteCommand<CMD_SelectObject>(ObjectSelection{});
	}

	void IDE::FocusOnSelectedGameObjects()
	{
		if (_selected_objects.game_objects.size())
		{
			vec3 finalCamPos{};
			for (const auto& go : _selected_objects.game_objects)
			{
				if (go)
					finalCamPos += go->GetComponent<Transform>()->GlobalPosition();
			}
			finalCamPos /= static_cast<float>(_selected_objects.game_objects.size());

			/*
			Needs to find how much space the object pixels takes on screen, then this distance is based on that.
			If single object, object pixels should take 50% of screen.
			If multiple objects, the ends of each object pixels on screen plus some padding should be used as anchor points to determine distance.
			But this is lazy method. 20.
			*/
			const float distanceFromObject = 20;
			const Handle<Transform> camTransform = Core::GetSystem<IDE>().GetEditorCamera()->GetGameObject()->GetComponent<Transform>();
			camTransform->position = finalCamPos - camTransform->Forward() * distanceFromObject;

			FindWindow<IGE_SceneView>()->SetOrbitPoint(finalCamPos);
		}
	}

	void IDE::CreateGameObject(Handle<GameObject> parent, string name, vector<string> initial_components)
	{
		auto* cmd = ExecuteCommand<CMD_CreateGameObject>(parent, std::move(name), std::move(initial_components));
		SelectGameObject(cmd->GetGameObject(), false, true);
		ExecuteCommand<CMD_CollateCommands>(2);

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
				ExecuteCommand<CMD_DeleteGameObject>(h);
				++execute_counter;
			}
		}

		Unselect(true); ++execute_counter;
		ExecuteCommand<CMD_CollateCommands>(execute_counter);
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
				auto* cmd = ExecuteCommand<CMD_CreateGameObject>(v);
				sel.game_objects.push_back(cmd->GetGameObject());
				++execute_counter;
			}
		}

		if (execute_counter)
		{
			SetSelection(sel);
			++execute_counter;
		}

		ExecuteCommand<CMD_CollateCommands>(execute_counter);
	}

	void IDE::RecursiveCollectObjects(Handle<GameObject> i, vector<RecursiveObjects>& vector_ref)
	{
		RecursiveObjects newObject{};
		newObject.original_handle = i;

		//Copy all components from this gameobject
		for (auto& c : i->GetComponents())
			newObject.vector_of_components.emplace_back((*c).copy());

		SceneManager& sceneManager = Core::GetSystem<SceneManager>();
		auto children = sceneManager.FetchSceneGraphFor(i);

		if (children)
		{
			//If there is no children, it will stop
			children.Visit([&](const Handle<GameObject>& handle, int) -> bool //Recurse through one level only
			{
				if (handle == i) //Skip parent
					return true;

				RecursiveCollectObjects(handle, newObject.children); //Depth first recursive
				return false;
			});
		}
		vector_ref.push_back(newObject);
	}

	void IDE::Play()
	{
		HotReloadDLL();
		FindWindow<IGE_InspectorWindow>()->Reset();
		Core::GetScheduler().SetPauseState(UnpauseAll);
		Core::GetSystem<mono::ScriptSystem>().run_scripts = true;
		Core::GetSystem<PhysicsSystem>().Reset();
		Core::GetSystem<AudioSystem>().SetSystemPaused(false);
		ImGui::SetWindowFocus(FindWindow<IGE_GameView>()->window_name);
		_game_running = true;
		if (_game_frozen)
			Pause();
	}

	void IDE::Pause()
	{
		Core::GetScheduler().SetPauseState(GamePause);
		Core::GetSystem<AudioSystem>().SetSystemPaused(true);
		_game_frozen = true;
	}

	void IDE::Unpause()
	{
		if (_game_running)
		{
			Core::GetScheduler().SetPauseState(UnpauseAll);
			Core::GetSystem<AudioSystem>().SetSystemPaused(false);
		}
		_game_frozen = false;
	}

	void IDE::Stop()
	{
		RestoreFromTemporaryScene();
		Core::GetSystem<mono::ScriptSystem>().run_scripts = false;
		Core::GetScheduler().SetPauseState(EditorPause);
		Core::GetSystem<AudioSystem>().SetSystemPaused(false);
		Core::GetSystem<AudioSystem>().StopAllAudio();
		Core::GetSystem<PhysicsSystem>().Reset();
		Core::GetSystem<NetworkSystem>().Disconnect();
		_game_running = false;
		_game_frozen = false;
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
		Core::GetSystem<IDE>()._command_controller.ClearUndoRedoStack();
		Core::GetSystem<IDE>()._selected_objects.game_objects.clear();
		Core::GetSystem<IDE>()._selected_objects.assets.clear();
	}

	void IDE::MaximizeWindow(IGE_IWindow* window)
	{
		_maximized_window = window;
	}

	void IDE::SetupEditorScene()
	{
		// create editor view
		_editor_view = Core::GetResourceManager().Create<RenderTarget>();
		auto sz = _editor_view->Size();
		_editor_view->Size(uvec2{ Core::GetSystem<Application>().GetScreenSize() });
		//this->FindWindow<IGE_Console>()->PushMessage(std::to_string(sz.x) + "," + std::to_string(sz.y));
		_editor_view->Name("Editor View");
		//editor_view->Size();
		// create editor camera
		RscHandle<Scene> scene{};
		{
			auto camera = scene->CreateGameObject();
			Handle<Camera> camHandle = camera->AddComponent<Camera>();
			camera->GetComponent<Name>()->name = "Editor Camera";
			camera->Transform()->position = vec3{ 0, 0, 5 };
			camHandle->far_plane = 80.f;
			camHandle->render_target = _editor_view;
			camHandle->clear = color{ 0.05f, 0.05f, 0.1f, 1.f };
			//if (Core::GetSystem<GraphicsSystem>().GetAPI() != GraphicsAPI::Vulkan)
				camHandle->clear = *Core::GetResourceManager().Load<CubeMap>("/engine_data/textures/skybox/space.png.cbm", false);

			_camera = camHandle;
		}
	}

}
