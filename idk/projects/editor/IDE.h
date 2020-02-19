//////////////////////////////////////////////////////////////////////////////////
//@file		IDE.h
//@author	Muhammad Izha B Rahim, Wei Xiang
//@param	Email : izha95\@hotmail.com
//@date		9 SEPT 2019
//@brief	

/*
Main Brain of the Game Editor. 

Accessible through Core::GetSystem<IDE>() [#include <IDE.h>]


*/
//////////////////////////////////////////////////////////////////////////////////


#pragma once
#include <editor/IEditor.h>
#include <editor/imgui_interface.h>
#include <editor/commands/CommandController.h>
#include <editor/Registry.h>
#include <editor/ObjectSelection.h>

#undef FindWindow

namespace idk
{
	//Forward Declarations
	class IGE_IWindow;
	class RenderTarget;

	enum class GizmoOperation
	{
		Null = 0,
		Translate,
		Rotate,
		Scale
	};

	enum class GizmoMode
	{
		Local,
		World
	};

	class IDE : public IEditor
	{
	public:
        constexpr static auto path_tmp = "/tmp";
        constexpr static auto path_idk_app_data = "/idk";
        constexpr static auto path_tmp_scene = "/tmp/tmp_scene.ids";

		RscHandle<Scene> curr_scene;
		Registry reg_scene{ "/user/LastScene.yaml" };

		// gizmos
		GizmoOperation	gizmo_operation = GizmoOperation::Translate;
		GizmoMode		gizmo_mode = GizmoMode::Local; //World is might not work properly for scaling for now.

		Signal<> OnSelectionChanged;

		IDE();

		// lifetime
		void Init() override;
		void LateInit() override;
		void EarlyShutdown() override;
		void Shutdown() override;
		void EditorUpdate() override;
		void EditorDraw() override;

		Handle<Camera> GetEditorCamera();

		void PollShortcutInputs();

		// IDE_Style
		void ApplyDefaultStyle();
		void ApplyDefaultColors();
		void LoadEditorFonts();

		// getters
        template <typename T> // move into .inl if there are more template fns
        T* FindWindow()
        {
            auto iter = _windows_by_type.find(reflect::typehash<T>());
            return iter != _windows_by_type.end() ? static_cast<T*>(iter->second) : nullptr;
        }
		RscHandle<RenderTarget> GetEditorRenderTarget() const { return _editor_view; }
		bool IsGameRunning() const { return _game_running; }
		bool IsGameFrozen() const { return _game_frozen; }

		template <typename Cmd, typename... Args>
		Cmd* ExecuteCommand(Args&&... args)
		{
			return static_cast<Cmd*>(_command_controller.ExecuteCommand(std::make_unique<Cmd>(std::forward<Args>(args)...)));
		}

		// selection
		const ObjectSelection& GetSelectedObjects();
		void SelectGameObject(Handle<GameObject> handle, bool multiselect = false, bool force = false);
		void SelectAsset(GenericResourceHandle handle, bool multiselect = false, bool force = false);
		void SetSelection(ObjectSelection selection, bool force = false);
		void Unselect(bool force = false);

		void FocusOnSelectedGameObjects();

		// game object operations
		void CreateGameObject(Handle<GameObject> parent = {}, string name = "", vector<string> initial_components = {});
		void DeleteSelectedGameObjects();
		void Copy();
		void Paste();

		void RecursiveCollectObjects(Handle<GameObject> i, vector<RecursiveObjects>& vector_ref); //i object to copy, vector_ref = vector to dump into

		// play/pause/stop operations
		void Play();
		void Pause();
		void Unpause();
		void Stop();

		void ClearScene();

		void MaximizeWindow(IGE_IWindow* window);

	private:
		unique_ptr<imgui_interface> _interface;
		Handle<Camera> _camera;
		CommandController _command_controller; //For editor commands

		// Editor Scene
		bool _game_running = false;
		bool _game_frozen = false;
		bool _scripts_changed = false;
		RscHandle<RenderTarget> _editor_view;
		void SetupEditorScene();

		//Editor Windows
		vector<unique_ptr<IGE_IWindow>>	_ige_windows;
		hash_table<size_t, IGE_IWindow*> _windows_by_type;
		IGE_IWindow* _maximized_window = nullptr;

		bool _show_demo_window = false;
        bool _closing = false;

		//For selecting and displaying in inspector.
		ObjectSelection _selected_objects{};

		//For copy commands
		vector<vector<RecursiveObjects>> _copied_game_objects; // A vector of data containing gameobject data.

		friend class CMD_SelectObject;
		friend class CMD_CollateCommands;
		friend class IGE_MainWindow;
		friend class IGE_IWindow;
	};
}