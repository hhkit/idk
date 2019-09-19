//////////////////////////////////////////////////////////////////////////////////
//@file		IGE_MainWindow.cpp
//@author	Muhammad Izha B Rahim
//@param	Email : izha95\@hotmail.com
//@date		10 SEPT 2019
//@brief	

/*
This window controls the top main menu bar. It also controls the docking
of the editor.
*/
//////////////////////////////////////////////////////////////////////////////////



#include "pch.h"
#include <editor/windows/IGE_MainWindow.h>
#include <app/Application.h>
#include <scene/SceneManager.h>
#include <editorstatic/imgui/imgui_internal.h> //DockBuilderDockNode
#include <editor/commands/CommandList.h> //DockBuilderDockNode
#include <iostream>
#include <IDE.h>

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


	void IGE_MainWindow::BeginWindow()
	{
		//ivec2 screen = Core::GetSystem<Application>().GetScreenSize();
		//
		//window_size = ImVec2{ (float)screen.x, (float)screen.y };
		//window_size = ImGui::GetWindowSize();
		//window_position = ImGui::GetWindowPos();
		ImGuiViewport* viewport = ImGui::GetMainViewport();

		//ImGui::SetNextWindowPos(viewport->Pos);
		window_size = viewport->Size;
		//ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::SetNextWindowBgAlpha(0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::PushStyleColor(ImGuiCol_MenuBarBg, ImVec4{ 0.92f, 0.92f, 0.92f, 1.0f }); //The File,Edit Tab

	}

	void IGE_MainWindow::EndWindow_V()
	{
		EndWindow();
		ImGuiViewport* viewport = ImGui::GetMainViewport();

		//Check if mouse is at this window or not
		is_mouse_hovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem);
		is_window_collapsed = ImGui::IsWindowCollapsed();
		//window_position = ImGui::GetWindowPos();
		//window_size = viewport->Size;

		itemCounter = 0;

		ImGui::End();

	}

	void IGE_MainWindow::DisplayFileMenu()
	{
		if (ImGui::BeginMenu("File")) {

			if (ImGui::MenuItem("New Scene", "CTRL+N")) {
				std::cout << "New Scene\n";
			}



			if (ImGui::MenuItem("Open Scene", "CTRL+O")) {
				std::cout << "Open Scene\n";


			} //Do something if pressed

			ImGui::Separator();


			if (ImGui::MenuItem("Save", "CTRL+S")) {

				std::cout << "Save current Scene\n";


			} //Do something if pressed



			if (ImGui::MenuItem("Save As...", "CTRL+SHIFT+S")) {

				std::cout << "Save Scene As...\n";



			} //Do something if pressed

			ImGui::Separator();
			if (ImGui::MenuItem("Exit", "ALT+F4")) {
				std::cout << "Quit Window\n";

			}
			ImGui::EndMenu(); //BeginMenu("File")
		}
	}

	void IGE_MainWindow::DisplayEditMenu()
	{
		if (ImGui::BeginMenu("Edit"))
		{
			CommandController& commandController = Core::GetSystem<IDE>().command_controller;
			bool canUndo = commandController.CanUndo();
			bool canRedo = commandController.CanRedo();
			if (ImGui::MenuItem("Undo", "CTRL+Z", nullptr, canUndo)) {
				commandController.UndoCommand();

			}


			if (ImGui::MenuItem("Redo", "CTRL+Y", nullptr, canRedo)) {
				commandController.RedoCommand();

			}


			ImGui::Separator();
			if (ImGui::MenuItem("Cut", "CTRL+X", nullptr, false)) {



			} //Do something if pressed
			if (ImGui::MenuItem("Copy", "CTRL+C", nullptr, false)) {



			} //Do something if pressed
			if (ImGui::MenuItem("Paste", "CTRL+V", nullptr, false)) {



			} //Do something if pressed

			ImGui::Separator();
			if (ImGui::MenuItem("Duplicate", "CTRL+D", nullptr, false)) {



			} //Do something if pressed
			if (ImGui::MenuItem("Delete")) {
				vector<Handle<GameObject>>& selected_gameObjects = Core::GetSystem<IDE>().selected_gameObjects;
				for (Handle<GameObject>& i : selected_gameObjects) {
					//Core::GetSystem<SceneManager>().GetActiveScene()->DestroyGameObject(i);
					commandController.ExecuteCommand(COMMAND(CMD_DeleteGameObject, i));
				}

				selected_gameObjects.clear();


			} //Do something if pressed

			ImGui::EndMenu(); //Close BeginMenu("Window")

		}
	}

	void IGE_MainWindow::DisplayGameObjectMenu()
	{
		if (ImGui::BeginMenu("GameObject"))
		{
			if (ImGui::MenuItem("Create Empty","CTRL+SHIFT+N")) {



			} //Do something if pressed



			ImGui::EndMenu(); 

		}
	}

	void IGE_MainWindow::DisplayComponentMenu()
	{
		if (ImGui::BeginMenu("Component"))
		{
			//Each button is disabled if gameobject is not selected!
			ImGui::EndMenu(); 

		}
	}

	void IGE_MainWindow::DisplayWindowMenu()
	{
		IDE& editor = Core::GetSystem<IDE>();
		static bool boolDemoWindow = false;

		if (ImGui::BeginMenu("Window"))
		{
			for (auto& i : editor.ige_windows) {
				ImGui::PushID(&i);
				if (ImGui::MenuItem(i->window_name, NULL, &i->is_open)) {
					//Do other stuff if needed
				}

				ImGui::PopID();
			}
			if (ImGui::MenuItem("ImGui Demo Window", NULL, &editor.bool_demo_window)) {

			}

			ImGui::EndMenu(); //Close BeginMenu("Window")
		}
	}

	void IGE_MainWindow::DisplayHelpMenu()
	{
		if (ImGui::BeginMenu("Help"))
		{

			

			ImGui::EndMenu(); //Close BeginMenu("Help")
		}


	}

	void IGE_MainWindow::DisplayToolBarChildWindow()
	{

		const ImVec2 toolBarSize{ window_size.x, toolBarHeight };

		ImGuiStyle& style = ImGui::GetStyle();

		ImGui::PushStyleColor(ImGuiCol_ChildBg, style.Colors[ImGuiCol_TitleBg]);


		//Tool bar
		ImGui::BeginChild("ToolBar", toolBarSize, false, childFlags);
		ImGui::PopStyleColor();


		const ImVec2 toolButtonSize = ImVec2{ 40.0f,20.0f };
		const ImVec2 toolButtonStartPos = ImVec2{ 6.0f,4.0f };

		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 2.0f); //Have the buttons look like buttons
		ImGui::SetCursorPos(toolButtonStartPos);

		GizmoOperation& gizmo_operation = Core::GetSystem<IDE>().gizmo_operation;

		ImVec4 activeColor = ImGui::GetStyle().Colors[ImGuiCol_ButtonActive];
		ImVec4 inactiveColor = ImGui::GetStyle().Colors[ImGuiCol_Button];

		ImGui::PushItemFlag(ImGuiItemFlags_Disabled, gizmo_operation == GizmoOperation_Null);
		ImGui::PushStyleColor(ImGuiCol_Button, gizmo_operation == GizmoOperation_Null ? activeColor : inactiveColor);
		if (ImGui::Button("Hand##Tool", toolButtonSize)) {
			gizmo_operation = GizmoOperation_Null;
		}
		ImGui::PopItemFlag();
		ImGui::PopStyleColor();

		ImGui::SetCursorPosX(toolButtonStartPos.x + toolButtonSize.x * 1);
		ImGui::SetCursorPosY(toolButtonStartPos.y);

		ImGui::PushItemFlag(ImGuiItemFlags_Disabled, gizmo_operation == GizmoOperation_Translate);
		ImGui::PushStyleColor(ImGuiCol_Button, gizmo_operation == GizmoOperation_Translate ? activeColor : inactiveColor);
		if (ImGui::Button("Move##Tool", toolButtonSize)) {
			gizmo_operation = GizmoOperation_Translate;
		}
		ImGui::PopItemFlag();
		ImGui::PopStyleColor();

		ImGui::SetCursorPosX(toolButtonStartPos.x + toolButtonSize.x * 2);
		ImGui::SetCursorPosY(toolButtonStartPos.y);

		ImGui::PushItemFlag(ImGuiItemFlags_Disabled, gizmo_operation == GizmoOperation_Rotate);
		ImGui::PushStyleColor(ImGuiCol_Button, gizmo_operation == GizmoOperation_Rotate ? activeColor : inactiveColor);
		if (ImGui::Button("Rotate##Tool", toolButtonSize)) {
			gizmo_operation = GizmoOperation_Rotate;
		}
		ImGui::PopItemFlag();
		ImGui::PopStyleColor();

		ImGui::SetCursorPosX(toolButtonStartPos.x + toolButtonSize.x * 3);
		ImGui::SetCursorPosY(toolButtonStartPos.y);

		ImGui::PushItemFlag(ImGuiItemFlags_Disabled, gizmo_operation == GizmoOperation_Scale);
		ImGui::PushStyleColor(ImGuiCol_Button, gizmo_operation == GizmoOperation_Scale ? activeColor : inactiveColor);
		if (ImGui::Button("Scale##Tool", toolButtonSize)) {
			gizmo_operation = GizmoOperation_Scale;
		}
		ImGui::PopItemFlag();
		ImGui::PopStyleColor();

		ImGui::PopStyleVar();

		ImGui::EndChild();

	}

	void IGE_MainWindow::DisplayHintBarChildWindow()
	{

		ImGuiStyle& style = ImGui::GetStyle();
		ImGui::SetCursorPosY(window_size.y - ImGui::GetFrameHeight()); //30 is child size, 18 is default font size

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(2.0f, 2.0f));
		ImGui::PushStyleColor(ImGuiCol_ChildBg, style.Colors[ImGuiCol_TitleBg]);

		ImGui::BeginChild("HintBar", ImVec2{ window_size.x, ImGui::GetFrameHeight() }, true, childFlags);

		ImGui::PopStyleVar();
		ImGui::PopStyleColor();


		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		//ImGui::TextUnformatted(hint_text_output.c_str());


		ImGui::EndChild();

	}

	void IGE_MainWindow::PollShortcutInput()
	{
		CommandController& commandController = Core::GetSystem<IDE>().command_controller;
		GizmoOperation& gizmo_operation = Core::GetSystem<IDE>().gizmo_operation;

		if (!ImGui::IsAnyMouseDown()) { //Disable shortcut whenever mouse is pressed

			//CTRL + Z (Careful, this clashes with CTRL +Z in ImGui::InputText() FIX TODO
			if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Z)) && ImGui::IsKeyDown(static_cast<int>(Key::Control))) {
				commandController.UndoCommand();
			}

			//CTRL + Y (Careful, this clashes with CTRL + Y in ImGui::InputText() FIX TODO
			if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Y)) && ImGui::IsKeyDown(static_cast<int>(Key::Control))) {
				commandController.RedoCommand();
			}

			//QWER = Move, Translate, Rotate, Scale (refer to ASCII Table)
			//Q = Move
			if (ImGui::IsKeyPressed(81)) {
				gizmo_operation = GizmoOperation_Null;
			}
			//W = Translate
			else if (ImGui::IsKeyPressed(87)) {
				gizmo_operation = GizmoOperation_Translate;
			}
			//E = Rotate
			else if (ImGui::IsKeyPressed(69)) {
				gizmo_operation = GizmoOperation_Rotate;
			}
			//R = Scale
			else if (ImGui::IsKeyPressed(82)) {
				gizmo_operation = GizmoOperation_Scale;
			}
			


		}
	}


	void IGE_MainWindow::Update() {

		ImGui::PopStyleVar(3); //Pop from BeginWindow()
		ImGui::PopStyleColor(); //Pop from BeginWindow()

		/*Main Menu Bar*/
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{ 0.0f, 0.0f, 0.0f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_PopupBg, ImVec4{ 0.92f, 0.92f, 0.92f, 1.0f }); //When you press File or Edit tab
		if (ImGui::BeginMenuBar()) {

			DisplayFileMenu();
			DisplayEditMenu();
			DisplayWindowMenu();
			DisplayGameObjectMenu();
			DisplayComponentMenu();
			DisplayHelpMenu();



			ImGui::EndMenuBar(); //MainMenuBar
		}
		ImGui::PopStyleColor(2);



		DisplayToolBarChildWindow();

		ImGui::SetCursorPosY(48.0f); //30 is child size, 18 is default font size
		ImGuiID dockspace_id = ImGui::GetID("IGEDOCKSPACE");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, window_size.y - toolBarHeight - (ImGui::GetFrameHeight()*2)), ImGuiDockNodeFlags_PassthruCentralNode);
		//Imgui internal
		//ImGui::DockBuilderDockWindow("SceneView", dockspace_id);

		DisplayHintBarChildWindow();

		PollShortcutInput();
		

	}







}
