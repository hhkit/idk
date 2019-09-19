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

	class IDE : public IEditor
	{
	public:
		IDE();

		void Init() override;
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

		unique_ptr<edt::I_Interface> _interface;

		//GraphicsAPI gLibVer;
		unique_ptr<IGE_MainWindow> ige_main_window		{};
		//vector <unique_ptr<IGE_IWindow>> ige_windows	{};

		bool bool_demo_window					 { false };
		CommandController command_controller			{};

		vector<Handle<GameObject>> selected_gameObjects {};

		vector<unique_ptr<IGE_IWindow>> ige_windows	    {};
		hash_table<size_t, IGE_IWindow*> windows_by_type{};

        bool closing = false;

	};
}