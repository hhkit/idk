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
#include <editor/windows/IGE_MainWindow.h>
#include <app/Application.h>
#include <iostream>

namespace idk {


	IGE_MainWindow::IGE_MainWindow()
		:IGE_IWindow{ "MainWindow",true,ImVec2{ 0,0 },ImVec2{ 0,0 } } {		//Delegate Constructor to set window size
			// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.
		window_flags = ImGuiWindowFlags_NoDocking 
					 | ImGuiWindowFlags_NoTitleBar
					 | ImGuiWindowFlags_NoCollapse
					 | ImGuiWindowFlags_NoResize 
					 | ImGuiWindowFlags_NoMove 
					 | ImGuiWindowFlags_NoBringToFrontOnFocus
					 | ImGuiWindowFlags_NoSavedSettings
					 | ImGuiWindowFlags_NoNavFocus 
					 | ImGuiWindowFlags_MenuBar 
					 | ImGuiWindowFlags_NoBackground;

		size_condition_flags = ImGuiCond_Always;
		pos_condition_flags = ImGuiCond_Always;
		
	}


	IGE_MainWindow::~IGE_MainWindow() {
	}

	void IGE_MainWindow::Initialize() {

	}


	void IGE_MainWindow::BeginWindow()
	{
		//ivec2 screen = Core::GetSystem<Application>().GetScreenSize();
		//
		//window_size = ImVec2{ (float)screen.x, (float)screen.y };
		//window_size = ImGui::GetWindowSize();
		//window_position = ImGui::GetWindowPos();
		ImGuiViewport* viewport = ImGui::GetMainViewport();

		//ImGui::SetNextWindowPos(viewport->Pos);
		//ImGui::SetNextWindowSize(viewport->Size, ImGuiCond_Always);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::SetNextWindowBgAlpha(0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));


	}

	void IGE_MainWindow::EndWindow_V()
	{
		EndWindow();
		ImGuiViewport* viewport = ImGui::GetMainViewport();

		//Check if mouse is at this window or not
		is_mouse_hovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem);
		is_window_collapsed = ImGui::IsWindowCollapsed();
		window_position = ImGui::GetWindowPos();
		window_size = viewport->Size;

		itemCounter = 0;

		ImGui::End();

	}


	void IGE_MainWindow::Update() {

		ImGui::PopStyleVar(3); //Pop from BeginWindow()

		ImGuiID dockspace_id = ImGui::GetID("IGEDOCKSPACE");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);


		/*Main Menu Bar*/
		if (ImGui::BeginMenuBar()) {
			if (ImGui::BeginMenu("File")) {

				if (ImGui::MenuItem("New Scene", "CTRL+SHIFT+N")) {
					std::cout << "New Scene\n";
				}



				if (ImGui::MenuItem("Load Scene...", "CTRL+SHIFT+O")) {
					std::cout << "Load Scene\n";


				} //Do something if pressed
				if (ImGui::MenuItem("Save current Scene", "CTRL+S")) {

					std::cout << "Save current Scene\n";


				} //Do something if pressed



				if (ImGui::MenuItem("Save Scene As...", "CTRL+SHIFT+S")) {

					std::cout << "Save Scene As...\n";



				} //Do something if pressed


				if (ImGui::MenuItem("Quit", "ALT+F4")) {
					std::cout << "Quit Window\n";

				}
				ImGui::EndMenu(); //BeginMenu("File")
			}

			if (ImGui::BeginMenu("Debug")) {
				//ImGui::MenuItem("Metrics", NULL, &editorRef.show_metrics_window);      // Edit bools storing our windows open/close state
				//ImGui::MenuItem("Demo Window", NULL, &editorRef.show_demo_window);      // Edit bools storing our windows open/close state
				ImGui::EndMenu(); //BeginMenu("Debug")
			}
			if (ImGui::BeginMenu("Window"))
			{
				//if (ImGui::Button("Reset Window Positions", ImVec2(180, 20))) {
				//
				//	editorRef.RepositionWindows();
				//
				//}

				//Windows
				//ImGui::MenuItem(editorRef.hierarchyWindow.windowName, NULL, &editorRef.hierarchyWindow.isOpen);						// Edit bools storing our windows open/close state
				//ImGui::MenuItem(editorRef.projectContentWindow.windowName, NULL, &editorRef.projectContentWindow.isOpen);			// Edit bools storing our windows open/close state
				//ImGui::MenuItem(editorRef.propertiesWindow.windowName, NULL, &editorRef.propertiesWindow.isOpen);
				//ImGui::MenuItem(editorRef.gameCameraWindow.windowName, NULL, &editorRef.gameCameraWindow.isOpen);						// Edit bools storing our windows open/close state
				//ImGui::MenuItem(editorRef.logWindow.windowName, NULL, &editorRef.logWindow.isOpen);						// Edit bools storing our windows open/close state



				ImGui::EndMenu(); //Close BeginMenu("Window")
			}

			//ImGui::MenuItem("Settings", NULL, &editorRef.settingsWindow.isOpen);


			//ImGui::MenuItem(editorRef.debugWindow.windowName, NULL, &editorRef.debugWindow.isOpen);      // Edit bools storing our windows open/close state


			if (ImGui::BeginMenu("Help"))
			{

				ImGui::MenuItem("Middle Mouse to drag camera.", 0, false, false);
				ImGui::MenuItem("F to focus on selected gameobject.", 0, false, false);
				ImGui::MenuItem("LMB on gamescreen to deselect gameobject.", 0, false, false);
				ImGui::MenuItem("CTRL+D to duplicate gameobject.", 0, false, false);
				ImGui::MenuItem("RMB to move gameobject.", 0, false, false);



				ImGui::EndMenu(); //Close BeginMenu("Help")
			}



			//if (ImGui::Button("Play", ImVec2{ 150,20 })) {

			//}

			if (ImGui::BeginMenu("Play"))
			{

				ImGui::EndMenu(); //Close BeginMenu("Play")
			}

			ImGui::SameLine();
			DrawHelpMarker("Shortcut: F1");

			ImGui::SameLine();

			ImGuiViewport* viewport = ImGui::GetMainViewport();
			
			//Draw FPS at menu bar at the top right
			ImGui::SameLine(viewport->Size.x - 90.0f);

			//Core::GetSystem<Application>().
			//ImGui::Text("FPS:%-.2f", editorRef.GetFPS());

			

			ImGui::EndMenuBar(); //MainMenuBar
		}

	}







}
