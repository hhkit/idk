//////////////////////////////////////////////////////////////////////////////////
//@file		IGE_MainWindow.cpp
//@author	Muhammad Izha B Rahim
//@param	Email : izha95\@hotmail.com
//@date		30 AUG 2019
//@brief	

/*
This window controls the top main menu bar. It also controls the docking
of the editor.
*/
//////////////////////////////////////////////////////////////////////////////////



#include "pch.h"
#include <editor/windows/IGE_ProjectWindow.h>
#include <app/Application.h>
#include <iostream>

namespace idk {

	IGE_ProjectWindow::IGE_ProjectWindow()
		:IGE_IWindow{ "Project##ProjectWindow",true,ImVec2{ 800,200 },ImVec2{ 200,200 } } {		//Delegate Constructor to set window size
			// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.
		window_flags = ImGuiWindowFlags_NoCollapse
					 | ImGuiWindowFlags_NoSavedSettings;

		//size_condition_flags = ImGuiCond_Always;
		//pos_condition_flags = ImGuiCond_Always;



	}

	void IGE_ProjectWindow::BeginWindow()
	{



	}
	void IGE_ProjectWindow::Update()
	{
		


	}

}
