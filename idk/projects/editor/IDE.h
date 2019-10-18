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

	class CameraControls;

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
		IDE();

		void Init() override;
		void LateInit() override;
		void Shutdown() override;
		void EditorUpdate() override;
		void EditorDraw() override;

		CameraControls& currentCamera();

        template <typename T> // move into .inl if there are more template fns
        T* FindWindow()
        {
            auto iter = windows_by_type.find(reflect::typehash<T>());
            return iter != windows_by_type.end() ? static_cast<T*>(iter->second) : nullptr;
        }

	private:
		friend class IGE_MainWindow;
		friend class IGE_SceneView;
		friend class IGE_ProjectWindow;
		friend class IGE_HierarchyWindow;
		friend class IGE_InspectorWindow;
		friend class CMD_DeleteGameObject;
		friend class CMD_CreateGameObject;
		friend class CommandController;

		unique_ptr<edt::I_Interface> _interface;
        string _editor_app_data;

		//GraphicsAPI gLibVer;
		unique_ptr<IGE_MainWindow> ige_main_window		{};
		//vector <unique_ptr<IGE_IWindow>> ige_windows	{};

		bool bool_demo_window					 { false };
		CommandController command_controller			{};

		vector<Handle<GameObject>> selected_gameObjects {};
		vector<mat4>			   selected_matrix		{}; //For selected_gameObjects
		void RefreshSelectedMatrix();

		vector<unique_ptr<IGE_IWindow>> ige_windows	    {};
		hash_table<size_t, IGE_IWindow*> windows_by_type{};

        bool closing = false;

		GizmoOperation gizmo_operation = GizmoOperation_Translate;
		MODE gizmo_mode = MODE::LOCAL; //World is might not work properly for scaling for now.


		void FocusOnSelectedGameObjects();
		vec3 focused_vector{}; //Updated everytime FocusOnSelectedGameObjects is called. For Scroll Vector WIP
		float scroll_multiplier					= 1.0f;
		const float default_scroll_multiplier	= 1.0f;			//When on focus, this resets the scroll_multiplier
		const float scroll_additive				= 0.90f;		//Amount of multiplication when scrolling farther.This adds to scroll_multiplier
		const float scroll_subtractive			= 0.85f;		//Amount of multiplication when scrolling nearer .This adds to scroll_multiplier

		bool flag_skip_render					= false;

		void RecursiveCollectObjects(Handle<GameObject> i, vector<RecursiveObjects>& vector_ref); //i object to copy, vector_ref = vector to dump into
		vector<vector<RecursiveObjects>> copied_gameobjects	{}; //A vector of data containing gameobject data.
		reflect::dynamic				 copied_component	{};
	};
}