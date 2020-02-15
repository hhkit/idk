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
#include <audio/AudioSystem.h> //AudioSystem
#include <PauseConfigurations.h>
#include <app/Application.h>
#include <proj/ProjectManager.h>
#include <imgui/ImGuizmo.h>
#include <script/ScriptSystem.h>
#include <ds/span.inl>
#include <anim/Animator.inl>
#include <core/Scheduler.inl>

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

			auto game_playing = Core::GetSystem<IDE>().IsGameRunning();

			if (ImGui::MenuItem("New Scene", "CTRL+N", false, !game_playing))
				NewScene();

			if (ImGui::MenuItem("Open Scene", "CTRL+O", false, !game_playing))
				OpenScene();

			ImGui::Separator();

			if (ImGui::MenuItem("Save", "CTRL+S", false, !game_playing))
				SaveScene();

			if (ImGui::MenuItem("Save As...", "CTRL+SHIFT+S", false, !game_playing))
				SaveSceneAs();

			ImGui::Separator();

			if (ImGui::MenuItem("Exit", "ALT+F4")) 
			{
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
			CommandController& commandController = Core::GetSystem<IDE>()._command_controller;
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
			if (ImGui::MenuItem("Copy", "CTRL+C", nullptr, false))
			{
				Core::GetSystem<IDE>().Copy();
			} 
			if (ImGui::MenuItem("Paste", "CTRL+V", nullptr, false)) 
			{
				Core::GetSystem<IDE>().Paste();
			}

			ImGui::Separator();
			if (ImGui::MenuItem("Duplicate", "CTRL+D", nullptr, false)) {



			} //Do something if pressed
			//if (ImGui::MenuItem("Delete")) {
			//	vector<Handle<GameObject>>& selected_gameObjects = Core::GetSystem<IDE>().GetSelectedObjects().game_objects;
			//	IDE& editor = Core::GetSystem<IDE>();
			//	int execute_counter = 0;
			//	while (!editor.selected_gameObjects.empty()) {
			//		Handle<GameObject> i = editor.selected_gameObjects.front();
			//		editor.selected_gameObjects.erase(editor.selected_gameObjects.begin());
			//		commandController.ExecuteCommand(COMMAND(CMD_DeleteGameObject, i));
			//		++execute_counter;
			//	}

			//	commandController.ExecuteCommand(COMMAND(CMD_CollateCommands, execute_counter));

			//	selected_gameObjects.clear();


			//} //Do something if pressed

			ImGui::EndMenu(); //Close BeginMenu("Window")

		}
	}

	void IGE_MainWindow::DisplayGameObjectMenu()
	{
		IDE& editor = Core::GetSystem<IDE>();
		
		if (ImGui::BeginMenu("GameObject"))
		{
			if (ImGui::MenuItem("Create Empty","CTRL+SHIFT+N")) 
			{
				editor.CreateGameObject();
			} 

			if (ImGui::MenuItem("Create Empty Child", "ALT+SHIFT+N")) 
			{
				if (editor.GetSelectedObjects().game_objects.size())
					editor.CreateGameObject(editor.GetSelectedObjects().game_objects.front());
				else
					editor.CreateGameObject();
			}

			ImGui::EndMenu();
		}
	}

	void IGE_MainWindow::DisplayComponentMenu()
	{
		if (ImGui::BeginMenu("Component"))
		{
			IDE& editor = Core::GetSystem<IDE>();
			bool canSelect = editor.GetSelectedObjects().game_objects.size();

			span componentNames = GameState::GetComponentNames();
			for (const char* name : componentNames)
			{
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

				if (ImGui::MenuItem(displayName.c_str(), nullptr, nullptr, canSelect)) 
				{
					int execute_counter = 0;
					for (Handle<GameObject> i : editor.GetSelectedObjects().game_objects) 
					{
						editor.ExecuteCommand<CMD_AddComponent>(i, string{ name });
						++execute_counter;
					}
					Core::GetSystem<IDE>().ExecuteCommand<CMD_CollateCommands>(execute_counter);
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
			for (auto& i : editor._ige_windows) {
				ImGui::PushID(&i);
				if (ImGui::MenuItem(i->window_name, NULL, &i->is_open)) {
					//Do other stuff if needed
				}

				ImGui::PopID();
			}
			if (ImGui::MenuItem("ImGui Demo Window", NULL, &editor._show_demo_window)) {

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

		ImGui::PushItemFlag(ImGuiItemFlags_Disabled, gizmo_operation == GizmoOperation::Null);
		ImGui::PushStyleColor(ImGuiCol_Button, gizmo_operation == GizmoOperation::Null ? activeColor : inactiveColor);
		if (ImGui::Button("Hand##Tool", toolButtonSize)) {
			gizmo_operation = GizmoOperation::Null;
		}
		ImGui::PopItemFlag();
		ImGui::PopStyleColor();

		ImGui::SetCursorPosX(toolButtonStartPos.x + toolButtonSize.x * 1);
		ImGui::SetCursorPosY(toolButtonStartPos.y);

		ImGui::PushItemFlag(ImGuiItemFlags_Disabled, gizmo_operation == GizmoOperation::Translate);
		ImGui::PushStyleColor(ImGuiCol_Button, gizmo_operation == GizmoOperation::Translate ? activeColor : inactiveColor);
		if (ImGui::Button("Move##Tool", toolButtonSize)) {
			gizmo_operation = GizmoOperation::Translate;
		}
		ImGui::PopItemFlag();
		ImGui::PopStyleColor();

		ImGui::SetCursorPosX(toolButtonStartPos.x + toolButtonSize.x * 2);
		ImGui::SetCursorPosY(toolButtonStartPos.y);

		ImGui::PushItemFlag(ImGuiItemFlags_Disabled, gizmo_operation == GizmoOperation::Rotate);
		ImGui::PushStyleColor(ImGuiCol_Button, gizmo_operation == GizmoOperation::Rotate ? activeColor : inactiveColor);
		if (ImGui::Button("Rotate##Tool", toolButtonSize)) {
			gizmo_operation = GizmoOperation::Rotate;
		}
		ImGui::PopItemFlag();
		ImGui::PopStyleColor();

		ImGui::SetCursorPosX(toolButtonStartPos.x + toolButtonSize.x * 3);
		ImGui::SetCursorPosY(toolButtonStartPos.y);

		ImGui::PushItemFlag(ImGuiItemFlags_Disabled, gizmo_operation == GizmoOperation::Scale);
		ImGui::PushStyleColor(ImGuiCol_Button, gizmo_operation == GizmoOperation::Scale ? activeColor : inactiveColor);
		if (ImGui::Button("Scale##Tool", toolButtonSize)) {
			gizmo_operation = GizmoOperation::Scale;
		}
		ImGui::PopItemFlag();
		ImGui::PopStyleColor();

		ImGui::SetCursorPosX(toolButtonStartPos.x + toolButtonSize.x * 6);
		ImGui::SetCursorPosY(toolButtonStartPos.y+3);

		auto& gizmo_mode = Core::GetSystem<IDE>().gizmo_mode;
		string localGlobal = gizmo_mode == GizmoMode::World ? "Global##Tool" : "Local##Tool";
		if (ImGui::Button(localGlobal.c_str(), ImVec2{ toolButtonSize.x+20.0f,toolButtonSize.y-6.0f })) {
			gizmo_mode = gizmo_mode == GizmoMode::World ? GizmoMode::Local : GizmoMode::World;
		}



        ImGui::SetCursorPosX(toolBarSize.x * 0.5f - toolButtonSize.x * 1.0f);
        ImGui::SetCursorPosY(toolButtonStartPos.y);
		ImGui::PushID(1337);
		if (!Core::GetSystem<IDE>().IsGameRunning())
		{
			if (ImGui::Button(ICON_FA_PLAY, toolButtonSize))
				Core::GetSystem<IDE>().Play();
		}
		else
		{
			ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetColorU32(ImGuiCol_ButtonActive));
			if (ImGui::Button(ICON_FA_PLAY, toolButtonSize))
				Core::GetSystem<IDE>().Stop();
			ImGui::PopStyleColor();
		}

		ImGui::SameLine(0, 0);

		if (!Core::GetSystem<IDE>().IsGameFrozen())
		{
			if (ImGui::Button(ICON_FA_PAUSE, toolButtonSize))
				Core::GetSystem<IDE>().Pause();
		}
		else
		{
			ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetColorU32(ImGuiCol_ButtonActive));
			if (ImGui::Button(ICON_FA_PAUSE, toolButtonSize))
				Core::GetSystem<IDE>().Unpause();
			ImGui::PopStyleColor();
		}
		ImGui::PopID();
		
		ImGui::PopStyleVar();

		// ImGui::SameLine();
		// if (ImGui::Button("Build Tree"))
		// 	Core::GetSystem<PhysicsSystem>().BuildStaticTree();
		// ImGui::SameLine();
		// if (ImGui::Button("Clear Tree"))
		// 	Core::GetSystem<PhysicsSystem>().ClearStaticTree();

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
			ImGui::DockBuilderDockWindow(ide.FindWindow<IGE_LightLister>()->window_name, bottom_left);
            ImGui::DockBuilderDockWindow(ide.FindWindow<IGE_ProjectWindow>()->window_name, bottom_left);
            ImGui::DockBuilderDockWindow(ide.FindWindow<IGE_HierarchyWindow>()->window_name, left);
            ImGui::DockBuilderDockWindow(ide.FindWindow<IGE_InspectorWindow>()->window_name, right);
            ImGui::DockBuilderFinish(dockspace_id);
        }

		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, window_size.y - toolBarHeight - (ImGui::GetFrameHeight()*2)), ImGuiDockNodeFlags_PassthruCentralNode);
		//Imgui internal
		//ImGui::DockBuilderDockWindow("SceneView", dockspace_id);

		DisplayHintBarChildWindow();

	}







}
