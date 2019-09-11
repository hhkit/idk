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

		vector <unique_ptr<IGE_IWindow>> ige_windows	{};

	};
}