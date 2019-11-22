//////////////////////////////////////////////////////////////////////////////////
//@file		IGE_MainWindow.cpp
//@author	Muhammad Izha B Rahim
//@param	Email : izha95\@hotmail.com
//@date		18 NOV 2019
//@brief	

/*
This window controls the top main menu bar. It also controls the docking
of the editor.
*/
//////////////////////////////////////////////////////////////////////////////////



#include "pch.h"
#include <editor/windows/IGE_MainWindow.h>
#include <editorstatic/imgui/imgui_internal.h> //DockBuilderDockNode
#include <editor/commands/CommandList.h> //Commands
#include <common/Transform.h> //DockBuilderDockNode
#include <common/TagManager.h>
#include <gfx/Camera.h> //DockBuilderDockNode
#include <iostream>
#include <app/Keys.h>
#include <IDE.h>
#include <editor/SceneManagement.h>
#include <editor/windows/IGE_WindowList.h>
#include <core/Scheduler.h>
#include <PauseConfigurations.h>
#include <app/Application.h>
#include <proj/ProjectManager.h>
#include <imgui/ImGuizmo.h>

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
		//ImGuiViewport* viewport = ImGui::GetMainViewport();

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

            if (ImGui::MenuItem("New Project"))
            {
                
            }

            if (ImGui::MenuItem("Open Project"))
            {
                const DialogOptions dialog{ "IDK Project", ProjectManager::ext };
                auto proj = Core::GetSystem<Application>().OpenFileDialog(dialog);
                if (proj)
                    Core::GetSystem<ProjectManager>().LoadProject(*proj);
            }

            ImGui::Separator();

			if (ImGui::MenuItem("New Scene", "CTRL+N"))
				NewScene();

			if (ImGui::MenuItem("Open Scene", "CTRL+O"))
				OpenScene();

			ImGui::Separator();

			if (ImGui::MenuItem("Save", "CTRL+S")) 
				SaveScene();



			if (ImGui::MenuItem("Save As...", "CTRL+SHIFT+S")) 
				SaveSceneAs();

			ImGui::Separator();
			if (ImGui::MenuItem("Exit", "ALT+F4")) {
				std::cout << "Quit Window\n";
				Core::Shutdown();
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
				IDE& editor = Core::GetSystem<IDE>();
				editor.copied_gameobjects.clear();
				for (auto& i : editor.selected_gameObjects) {
					vector<RecursiveObjects> newObject{};
					editor.RecursiveCollectObjects(i, newObject);
					editor.copied_gameobjects.push_back(std::move(newObject));
				}

			} 
			if (ImGui::MenuItem("Paste", "CTRL+V", nullptr, false)) {
				IDE& editor = Core::GetSystem<IDE>();
				int execute_counter = 0;

				for (auto& i : editor.copied_gameobjects) {
					commandController.ExecuteCommand(COMMAND(CMD_CreateGameObject, i));
					++execute_counter;
				}

				commandController.ExecuteCommand(COMMAND(CMD_CallCommandAgain, execute_counter));

			}

			ImGui::Separator();
			if (ImGui::MenuItem("Duplicate", "CTRL+D", nullptr, false)) {



			} //Do something if pressed
			if (ImGui::MenuItem("Delete")) {
				vector<Handle<GameObject>>& selected_gameObjects = Core::GetSystem<IDE>().selected_gameObjects;
				IDE& editor = Core::GetSystem<IDE>();
				int execute_counter = 0;
				while (!editor.selected_gameObjects.empty()) {
					Handle<GameObject> i = editor.selected_gameObjects.front();
					editor.selected_gameObjects.erase(editor.selected_gameObjects.begin());
					commandController.ExecuteCommand(COMMAND(CMD_DeleteGameObject, i));
					++execute_counter;
				}

				commandController.ExecuteCommand(COMMAND(CMD_CallCommandAgain, execute_counter));

				selected_gameObjects.clear();


			} //Do something if pressed

			ImGui::EndMenu(); //Close BeginMenu("Window")

		}
	}

	void IGE_MainWindow::DisplayGameObjectMenu()
	{

		IDE& editor = Core::GetSystem<IDE>();
		
		if (ImGui::BeginMenu("GameObject"))
		{
			if (ImGui::MenuItem("Create Empty","CTRL+SHIFT+N")) {

				editor.command_controller.ExecuteCommand(COMMAND(CMD_CreateGameObject));

			} 
			if (ImGui::MenuItem("Create Empty Child", "ALT+SHIFT+N")) {
				if (editor.selected_gameObjects.size()) {
					editor.command_controller.ExecuteCommand(COMMAND(CMD_CreateGameObject, editor.selected_gameObjects.front()));

				}
				else {
					editor.command_controller.ExecuteCommand(COMMAND(CMD_CreateGameObject));
				}


			}


			ImGui::EndMenu(); 

		}
	}

	void IGE_MainWindow::DisplayComponentMenu()
	{
		if (ImGui::BeginMenu("Component"))
		{
			IDE& editor = Core::GetSystem<IDE>();
			bool canSelect = editor.selected_gameObjects.size() == 0 ? false : true;

			span componentNames = GameState::GetComponentNames();
			for (const char* name : componentNames) {
				string displayName = name;
				if (displayName == "Transform")
					continue;
				if (displayName == "Name")
					continue;

				//Comment/Uncomment this to remove text fluff 
				const string fluffText{ "idk::" };

				std::size_t found = displayName.find(fluffText);
				if (found != std::string::npos)
					displayName.erase(found, fluffText.size());

				/*
				const string fluffText2{ ">(void)" };
				found = displayName.find(fluffText2);
				if (found != std::string::npos)
					displayName.erase(found, fluffText2.size());

				*/

				if (ImGui::MenuItem(displayName.c_str(),nullptr,nullptr,canSelect)) {
					//Add component
					int execute_counter = 0;
					for (Handle<GameObject> i : editor.selected_gameObjects) {
						editor.command_controller.ExecuteCommand(COMMAND(CMD_AddComponent, i, string{ name }));
						++execute_counter;
					}

					CommandController& commandController = Core::GetSystem<IDE>().command_controller;
					commandController.ExecuteCommand(COMMAND(CMD_CallCommandAgain, execute_counter));
				}
			}
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

		ImGui::SetCursorPosX(toolButtonStartPos.x + toolButtonSize.x * 6);
		ImGui::SetCursorPosY(toolButtonStartPos.y+3);

		MODE& gizmo_mode = Core::GetSystem<IDE>().gizmo_mode;
		string localGlobal = gizmo_mode == WORLD ? "Global##Tool" : "Local##Tool";
		if (ImGui::Button(localGlobal.c_str(), ImVec2{ toolButtonSize.x+20.0f,toolButtonSize.y-6.0f })) {
			gizmo_mode = gizmo_mode == WORLD ? LOCAL : WORLD;
		}



        ImGui::SetCursorPosX(toolBarSize.x * 0.5f - toolButtonSize.x * 1.5f);
        ImGui::SetCursorPosY(toolButtonStartPos.y);
		if (Core::GetSystem<IDE>().game_running == false)
		{
			if (ImGui::Button("Play", toolButtonSize))
			{
				// IDE& editor = Core::GetSystem<IDE>();
				// for (auto& i : editor.ige_windows)
				// 	if (i->window_name != "Game")
				// 		i->is_open = false;
				// editor.currentCamera().current_camera->enabled = false;
				SaveSceneTemporarily();
				Core::GetScheduler().SetPauseState(UnpauseAll);
				Core::GetSystem<IDE>().game_running = true;
				Core::GetSystem<IDE>().game_frozen = false;
				Core::GetSystem<PhysicsSystem>().Reset();
			}
			ImGui::SameLine(0, 0);
			if (ImGui::Button("Reload DLL", toolButtonSize))
				HotReloadDLL();
		}
		else
		{
			if (Core::GetSystem<IDE>().game_frozen == false)
			{
				if (ImGui::Button("Pause", toolButtonSize))
				{
					Core::GetScheduler().SetPauseState(EditorPause);
					Core::GetSystem<IDE>().game_frozen = true;
				}
			}
			else
			{
				if (ImGui::Button("Unpause", toolButtonSize))
				{
					Core::GetScheduler().SetPauseState(UnpauseAll); 
					Core::GetSystem<IDE>().game_frozen = false;
				}
			}
			ImGui::SameLine(0, 0);
			if (ImGui::Button("Stop", toolButtonSize))
			{
				RestoreFromTemporaryScene();
				Core::GetScheduler().SetPauseState(EditorPause);
				Core::GetSystem<IDE>().game_running = false;
			}
		}

		
		ImGui::PopStyleVar();

        ImGui::SameLine(ImGui::GetWindowContentRegionWidth() -
            ImGui::CalcTextSize("Draw All Colliders").x - ImGui::GetStyle().FramePadding.y * 2 - ImGui::GetTextLineHeight() - ImGui::GetStyle().ItemSpacing.x * 2);

        ImGui::Checkbox("Draw All Colliders", &Core::GetSystem<PhysicsSystem>().debug_draw_colliders);

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

		if (ImGui::IsAnyItemActive()) { //Do not do any shortcuts when inputs are active! EG: Editing texts!
			return;
		}
		IDE& editor = Core::GetSystem<IDE>();
		CommandController& commandController = editor.command_controller;
		GizmoOperation& gizmo_operation = editor.gizmo_operation;

		if (!ImGui::IsAnyMouseDown()) { //Disable shortcut whenever mouse is pressed

			//CTRL + Z
			if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Z)) && ImGui::IsKeyDown(static_cast<int>(Key::Control))) {
				commandController.UndoCommand();
			}

			//CTRL + Y
			if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Y)) && ImGui::IsKeyDown(static_cast<int>(Key::Control))) {
				commandController.RedoCommand();
			}

			//CTRL + C
			if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_C)) && ImGui::IsKeyDown(static_cast<int>(Key::Control))) {
				editor.copied_gameobjects.clear();
				for (auto& i : editor.selected_gameObjects) {
					vector<RecursiveObjects> newObject{};
					editor.RecursiveCollectObjects(i, newObject);
					editor.copied_gameobjects.push_back(std::move(newObject));
				}
			}

			//CTRL + V
			if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_V)) && ImGui::IsKeyDown(static_cast<int>(Key::Control))) {
				int execute_counter = 0;
				for (auto& i : editor.copied_gameobjects) {
					commandController.ExecuteCommand(COMMAND(CMD_CreateGameObject,i));
					++execute_counter;
				}

				commandController.ExecuteCommand(COMMAND(CMD_CallCommandAgain, execute_counter));
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
			
			//DEL = Delete
			else if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Delete))) {
				int execute_counter = 0;
				//Move the gizmo away before deleting
				CameraControls&		main_camera = editor._interface->Inputs()->main_camera;
				Handle<Camera>		currCamera = main_camera.current_camera;
				Handle<Transform>	tfm = currCamera->GetGameObject()->GetComponent<Transform>();
				const auto			view_mtx = currCamera->ViewMatrix();
				const float*		viewMatrix = view_mtx.data();
				const auto			pers_mtx = currCamera->ProjectionMatrix();
				const float*		projectionMatrix = pers_mtx.data();
				float				gizmo_matrix[16]{0};
				ImGuizmo::Manipulate(viewMatrix, projectionMatrix, ImGuizmo::TRANSLATE, ImGuizmo::MODE::WORLD, gizmo_matrix, NULL, NULL);



				while (!editor.selected_gameObjects.empty()) {
					Handle<GameObject> i = editor.selected_gameObjects.front();
					editor.selected_gameObjects.erase(editor.selected_gameObjects.begin());
					commandController.ExecuteCommand(COMMAND(CMD_DeleteGameObject, i));
					++execute_counter;
				}

				commandController.ExecuteCommand(COMMAND(CMD_CallCommandAgain, execute_counter));
			}

			//F = Focus on GameObject
			if (ImGui::IsKeyPressed(70))
			{
				editor.FocusOnSelectedGameObjects();

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

        if (ImGui::DockBuilderGetNode(dockspace_id) == nullptr)
        {
            ImGui::DockBuilderRemoveNode(dockspace_id); // Clear out existing layout
            ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_CentralNode); // Add empty node
            ImGui::DockBuilderSetNodeSize(dockspace_id, window_size);

            ImGuiID main = dockspace_id;

            const float Y_RATIO = 0.3f;
            const float X_RATIO = 0.2f;

            ImGuiID right = ImGui::DockBuilderSplitNode(main, ImGuiDir_Right, X_RATIO, nullptr, &main);

            // bottom left: project
            ImGuiID bottom_left = ImGui::DockBuilderSplitNode(main, ImGuiDir_Down, Y_RATIO, nullptr, &main);

            // left pane: hierarchy
            ImGuiID left = ImGui::DockBuilderSplitNode(main, ImGuiDir_Left, X_RATIO / (1.0f - X_RATIO), nullptr, &main);

            auto& ide = Core::GetSystem<IDE>();
			ImGui::DockBuilderDockWindow(ide.FindWindow<IGE_GameView>()->window_name, main);
			ImGui::DockBuilderDockWindow(ide.FindWindow<IGE_SceneView>()->window_name, main);
			ImGui::DockBuilderDockWindow(ide.FindWindow<IGE_Console>()->window_name, bottom_left);
			ImGui::DockBuilderDockWindow(ide.FindWindow<IGE_ProgrammerConsole>()->window_name, bottom_left);
            ImGui::DockBuilderDockWindow(ide.FindWindow<IGE_ProjectWindow>()->window_name, bottom_left);
            ImGui::DockBuilderDockWindow(ide.FindWindow<IGE_HierarchyWindow>()->window_name, left);
            ImGui::DockBuilderDockWindow(ide.FindWindow<IGE_InspectorWindow>()->window_name, right);
            ImGui::DockBuilderFinish(dockspace_id);
        }

		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, window_size.y - toolBarHeight - (ImGui::GetFrameHeight()*2)), ImGuiDockNodeFlags_PassthruCentralNode);
		//Imgui internal
		//ImGui::DockBuilderDockWindow("SceneView", dockspace_id);

		DisplayHintBarChildWindow();

		PollShortcutInput();
		

	}







}
