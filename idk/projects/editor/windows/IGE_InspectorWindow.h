//////////////////////////////////////////////////////////////////////////////////
//@file		IGE_InspectorWindow.h
//@author	Muhammad Izha B Rahim
//@param	Email : izha95\@hotmail.com
//@date		6 SEPT 2019
//@brief	

/*
This window displays the editor window where you can select and modify gameobjects.
*/
//////////////////////////////////////////////////////////////////////////////////



#pragma once
#include <editor/windows/IGE_IWindow.h>

namespace idk {

	class IGE_InspectorWindow :
		public IGE_IWindow
	{
	public:
		IGE_InspectorWindow();

		virtual void BeginWindow() override;
		virtual void Update() override;

	
	protected:


	private:

		void DisplayFileMenu();
		void DisplayEditMenu();
		void DisplayGameObjectMenu();
		void DisplayComponentMenu();
		void DisplayWindowMenu();
		void DisplayHelpMenu();

		void DisplayToolBarChildWindow();
		void DisplayHintBarChildWindow();


		const ImGuiWindowFlags childFlags = ImGuiWindowFlags_NoTitleBar
										  | ImGuiWindowFlags_NoScrollbar
										  | ImGuiWindowFlags_NoResize
										  | ImGuiWindowFlags_NoSavedSettings
										  | ImGuiWindowFlags_NoMove
										  | ImGuiWindowFlags_NoDocking
										  | ImGuiWindowFlags_NoCollapse;

		const float toolBarHeight = 30.0f;

	};





}