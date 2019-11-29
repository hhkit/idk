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
#include <editor/ImGui_Interface.h>
#include <editor/commands/CommandController.h>
#include <editor/Registry.h>

#undef FindWindow



namespace idk
{
	//Forward Declarations
	class IGE_IWindow; 

	class IGE_MainWindow;
	class IGE_SceneView;
	class IGE_ProjectWindow;
	class IGE_HierarchyWindow;
	class IGE_InspectorWindow;
	class IGE_AnimatorWindow;

	class CameraControls;

	class RenderTarget;

	enum GizmoOperation {
		GizmoOperation_Null = 0,
		GizmoOperation_Translate,
		GizmoOperation_Rotate,
		GizmoOperation_Scale
	};
	enum MODE
	{
		LOCAL,
		WORLD
	};

	class IDE : public IEditor
	{
	public:
        constexpr static auto path_tmp = "/tmp";
        constexpr static auto path_idk_app_data = "/idk";
		
		IDE();

		void Init() override;
		void LateInit() override;
		void EarlyShutdown() override;
		void Shutdown() override;
		void EditorUpdate() override;
		void EditorDraw() override;

		CameraControls& currentCamera();

		void ClearScene();
		string_view GetTmpSceneMountPath() const;

        template <typename T> // move into .inl if there are more template fns
        T* FindWindow()
        {
            auto iter = windows_by_type.find(reflect::typehash<T>());
            return iter != windows_by_type.end() ? static_cast<T*>(iter->second) : nullptr;
        }

		RscHandle<RenderTarget> GetEditorRenderTarget() { return editor_view; };
		void ApplyDefaultColors();

        Registry reg_scene{ "/user/LastScene.yaml" };

	private:
		friend class IGE_MainWindow;
		friend class IGE_SceneView;
		friend class IGE_ProjectWindow;
		friend class IGE_HierarchyWindow;
		friend class IGE_InspectorWindow;
		friend class IGE_AnimatorWindow;
		friend class CMD_DeleteGameObject;
		friend class CMD_CreateGameObject;
		friend class CMD_CallCommandAgain;
		friend class CommandController;

		unique_ptr<edt::I_Interface> _interface;

		// Editor Scene
		bool game_running = false;
		bool game_frozen = true;
		RscHandle<Scene> curr_scene;
		RscHandle<RenderTarget> editor_view;
		void SetupEditorScene();

		//Editor Windows
		unique_ptr<IGE_MainWindow>			ige_main_window		{};
		vector<unique_ptr<IGE_IWindow>>		ige_windows			{};
		hash_table<size_t, IGE_IWindow*>	windows_by_type		{};

		CommandController command_controller			{}; //For editor commands

		bool bool_demo_window					 { false };

        bool closing			= false;
		bool flag_skip_render	= false;

		//For Gizmo controls
		GizmoOperation	gizmo_operation = GizmoOperation_Translate;
		MODE			gizmo_mode		= MODE::LOCAL; //World is might not work properly for scaling for now.

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
		vector<Handle<GameObject>> selected_gameObjects{};
		vector<mat4>			   selected_matrix{}; //For selected_gameObjects
		void RefreshSelectedMatrix();

		//For copy commands
		void RecursiveCollectObjects(Handle<GameObject> i, vector<RecursiveObjects>& vector_ref); //i object to copy, vector_ref = vector to dump into
		vector<vector<RecursiveObjects>> copied_gameobjects	{}; //A vector of data containing gameobject data.
		reflect::dynamic				 copied_component	{};
	};
}