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
#include <editor/CameraControls.h>
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
            auto iter = windows_by_type.find(reflect::typehash<T>());
            return iter != windows_by_type.end() ? static_cast<T*>(iter->second) : nullptr;
        }
		RscHandle<RenderTarget> GetEditorRenderTarget() const { return editor_view; }
		bool IsGameRunning() const { return game_running; }

		// selection
		const ObjectSelection& GetSelectedObjects();
		void SelectGameObject(Handle<GameObject> handle, bool multiselect = false, bool force = false);
		void SelectAsset(GenericResourceHandle handle, bool multiselect = false, bool force = false);
		void SetSelection(ObjectSelection selection, bool force = false);
		void Unselect(bool force = false);

		// game object operations
		void CreateGameObject(Handle<GameObject> parent = {}, string name = "", vector<string> initial_components = {});
		void DeleteSelectedGameObjects();
		void Copy();
		void Paste();

		void ClearScene();

	private:
		unique_ptr<imgui_interface> _interface;
		CameraControls _camera;
		CommandController command_controller; //For editor commands

		// Editor Scene
		bool game_running = false;
		bool game_frozen = true;
		bool scripts_changed = false;
		RscHandle<RenderTarget> editor_view;
		void SetupEditorScene();

		//Editor Windows
		vector<unique_ptr<IGE_IWindow>>	ige_windows;
		hash_table<size_t, IGE_IWindow*> windows_by_type;

		bool bool_demo_window = false;
        bool closing = false;

		//For Gizmo controls
		void FocusOnSelectedGameObjects();

		//Scrolling
		float		scroll_multiplier			= 2.0f;			//AFfects pan and scrolling intensity
		const float default_scroll_multiplier	= 2.0f;			//When on focus, this resets the scroll_multiplier
		const float scroll_additive				= 0.90f;		//Amount of multiplication when scrolling farther.This adds to scroll_multiplier
		const float scroll_subtractive			= 0.2f;			//Amount of multiplication when scrolling nearer .This adds to scroll_multiplier
		const float scroll_max					= 10.0f;
		const float scroll_min					= 0.1f;
		void IncreaseScrollPower();
		void DecreaseScrollPower();

		//For selecting and displaying in inspector.
		ObjectSelection				_selected_objects{};

		//For copy commands
		void RecursiveCollectObjects(Handle<GameObject> i, vector<RecursiveObjects>& vector_ref); //i object to copy, vector_ref = vector to dump into
		vector<vector<RecursiveObjects>> _copied_game_objects; // A vector of data containing gameobject data.



		friend class IGE_MainWindow;
		friend class IGE_SceneView;
		friend class IGE_ProjectWindow;
		friend class IGE_HierarchyWindow;
		friend class IGE_InspectorWindow;
		friend class IGE_AnimatorWindow;
		friend class IGE_LightLister;
		friend class CMD_DeleteGameObject;
		friend class CMD_CreateGameObject;
		friend class CMD_CollateCommands;
		friend class CMD_SelectObject;
		friend class CommandController;
	};
}