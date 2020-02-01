//////////////////////////////////////////////////////////////////////////////////
//@file		IGE_MainWindow.h
//@author	Muhammad Izha B Rahim
//@param	Email : izha95\@hotmail.com
//@date		9 SEPT 2019
//@brief	

/*
This window controls the top main menu bar. It also controls the docking
of the editor and polls of any shortcut used.
*/
//////////////////////////////////////////////////////////////////////////////////



#pragma once
#include <editor/windows/IGE_IWindow.h>

namespace idk {
	class IGE_MainWindow :
		public IGE_IWindow
	{
	public:
		IGE_MainWindow();

		virtual void Update() override;

		string hint_text_output	{"Place hint text here"}; //This is modified

	protected:
		virtual void BeginWindow() override;
		virtual void EndWindow_V() override;
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