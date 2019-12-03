//////////////////////////////////////////////////////////////////////////////////
//@file		IGE_HierarchyWindow.cpp
//@author	Muhammad Izha B Rahim
//@param	Email : izha95\@hotmail.com
//@date		03 DEC 2019
//@brief	

/*
This window controls the top main menu bar. It also controls the docking
of the editor.
*/
//////////////////////////////////////////////////////////////////////////////////



#include "pch.h"
#include <editor/windows/IGE_HierarchyWindow.h>
#include <editor/commands/CommandList.h>		//Commands
#include <editorstatic/imgui/imgui_internal.h> //InputTextEx
#include <editor/DragDropTypes.h>
#include <app/Application.h>
#include <core/GameObject.h>
#include <common/Name.h>
#include <common/Transform.h>
#include <prefab/PrefabInstance.h>
#include <gfx/Camera.h>
#include <core/Core.h>
#include <prefab/PrefabUtility.h>
#include <IDE.h>		//IDE
#include <iostream>

namespace idk {

	IGE_HierarchyWindow::IGE_HierarchyWindow()
		:IGE_IWindow{ "Hierarchy##IGE_HierarchyWindow",true,ImVec2{ 300,600 },ImVec2{ 150,150 } } {		//Delegate Constructor to set window size
			// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.
		window_flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_HorizontalScrollbar;


	}

	void IGE_HierarchyWindow::BeginWindow()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2{150.0f,100.0f});

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));


	}
	void IGE_HierarchyWindow::Update()
	{
		
		ImGui::PopStyleVar(2);


		ImGuiStyle& style = ImGui::GetStyle();

		ImGui::PushStyleColor(ImGuiCol_ChildBg, style.Colors[ImGuiCol_TitleBgActive]);

		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

		//MenuBar
        ImGui::BeginMenuBar();
		ImGui::PopStyleColor();
		ImGui::PopStyleVar(3);

		ImGui::SetCursorPosX(5);
		if (ImGui::Button("Create")) {
			ImGui::OpenPopup("CreatePopup");

		}
		if (ImGui::BeginPopup("CreatePopup")) {
            IDE& editor = Core::GetSystem<IDE>();

			if (ImGui::MenuItem("Create Empty", "CTRL+SHIFT+N")) {
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
            if (ImGui::BeginMenu("UI")) {
				const auto go = editor.selected_gameObjects.empty()? Handle<GameObject>() :editor.selected_gameObjects.front();

				auto parent = go;
				while (parent)
				{
					if (parent->GetComponent<Canvas>())
						break;
					parent = go->Parent();
				}
				if (!parent)
				{
					if (ImGui::MenuItem("Canvas"))
					{
						editor.command_controller.ExecuteCommand(COMMAND(CMD_CreateGameObject, go, "Canvas", vector<string>{ "RectTransform", "Canvas" }));
					}
				}
				if (ImGui::MenuItem("Image"))
					editor.command_controller.ExecuteCommand(COMMAND(CMD_CreateGameObject, go, "Image", vector<string>{ "RectTransform", "Image" }));
				if (ImGui::MenuItem("Text"))
					editor.command_controller.ExecuteCommand(COMMAND(CMD_CreateGameObject, go, "Text", vector<string>{ "RectTransform", "Text" }));
				
				ImGui::EndMenu();
            }

			ImGui::EndPopup();
		}
		if (ImGui::Button("...##AdditionalStuff")) {
			ImGui::OpenPopup("OpenAdditionalStuff");

		}
		if (ImGui::BeginPopup("OpenAdditionalStuff")) {
			ImGui::Checkbox("Show Editor Objects", &show_editor_objects);
			ImGui::EndPopup();
		}


		ImGui::SameLine();
		ImVec2 startPos = ImGui::GetCursorPos();
		ImGui::SetCursorPosX(startPos.x+15);

		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 12.0f);
		const auto search_bar_offset_x = ImGui::GetCursorPosX() + 20;
		//Can call textFilter.Draw(), but I want to use a custom inputText.
		bool value_changed = ImGui::InputTextEx("##ToolBarSearchBar", NULL, textFilter.InputBuf, IM_ARRAYSIZE(textFilter.InputBuf), ImVec2{ window_size.x - search_bar_offset_x,ImGui::GetFrameHeight() - 2 }, ImGuiInputTextFlags_None);
		if (value_changed)
			textFilter.Build();

		if (!ImGui::IsItemActive())
			DrawToolTipOnHover("Type here to filter gameobjects.\n" "Add a '-' to exclude gameobjects.");
		
		ImGui::PopStyleVar();

		ImGui::EndMenuBar();

		//Hierarchy Display
		SceneManager& sceneManager = Core::GetSystem<SceneManager>();
		SceneManager::SceneGraph& sceneGraph = sceneManager.FetchSceneGraph();
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0.0f,0.0f });
		//ImGui::Checkbox("Show Editor Objs", &show_editor_objects);
		
		//To unindent the first gameobject which is the scene
		ImGui::Unindent();

		int selectedCounter = 0; // This is for Shift Select. This iterates.
		int selectedItemCounter = 0; // This is for Shift Select. This is assigned
		bool isShiftSelectCalled = false;
		bool hasSelected_GameobjectsModified = false;
		int focused_gameobject_position =  0 ; //If it is -1, it means the ScrollToGameObjectInHierarchy is not called.
		int total_gameobjects_displayed = 0; //This is for the focused thing.
		bool is_scroll_focused_gameObject_found = false;
		vector<int> itemToSkipInGraph{};
		//Refer to TestSystemManager.cpp


        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4.0f, 1.0f));

		//Display gameobjects
		sceneGraph.visit([&](const Handle<GameObject>& handle, int depth) -> bool {

			if (depth > 0) {
				for (int i = 0; i < depth; ++i)
					ImGui::Indent();
			}
			else {
				for (int i = depth; i < 0; ++i)
					ImGui::Unindent();
			}
			if (!handle) //Ignore handle zero
				return true;

			if (!show_editor_objects && handle.scene == Scene::editor || handle.scene == Scene::prefab) { // ignore editor
				++selectedCounter; //counter here is for shift selecting

				return true;
			}

			vector<Handle<GameObject>>& selected_gameObjects = Core::GetSystem<IDE>().selected_gameObjects;

			ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow| ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_SpanFullWidth;

			SceneManager& sceneManager = Core::GetSystem<SceneManager>();
			SceneManager::SceneGraph* children = sceneManager.FetchSceneGraphFor(handle);
			if (children->size() == 0) {
				nodeFlags |= ImGuiTreeNodeFlags_Leaf;
			}

			bool is_its_child_been_selected = false;
			//Check if gameobject has been selected. Causes Big-O(n^2)
			for (Handle<GameObject>& i : selected_gameObjects) {
				if (handle == i) {
					nodeFlags |= ImGuiTreeNodeFlags_Selected;
					break;
				}
				else if (CheckIfChildrenIsSelected(children, i)) {
					is_its_child_been_selected = true;
					nodeFlags |= ImGuiTreeNodeFlags_Selected;

				}

			}

			Handle<Name> c_name = handle->GetComponent<Name>();
			string goName{};
			if (c_name)
				goName = c_name->name;
			
			const bool isNameEmpty = goName.empty();
            const bool is_prefab = handle->HasComponent<PrefabInstance>();
            ImColor col = ImGui::GetColorU32(ImGuiCol_Text);
			if (isNameEmpty) {
				goName = "Unnamed (";
				goName.append(serialize_text(handle.id));
				goName.append(")");
				//Draw Node. Trees will always return true if open, so use IsItemClicked to set object instead!
                col = ImVec4(0.75f, 0.75f, 0.75f, 1.0f);
			}
            if (is_prefab)
                col = handle->GetComponent<PrefabInstance>()->prefab ? style.Colors[ImGuiCol_PlotLinesHovered] : ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
            if (!handle->ActiveInHierarchy())
                col.Value.w = 0.5f;

            ImGui::PushStyleColor(ImGuiCol_Text, col.Value);
			
			if (!textFilter.PassFilter(goName.c_str())) {
                ImGui::PopStyleColor();
				++selectedCounter; // counter here is for shift selecting
				return true;
			}
			
			string idString = std::to_string(handle.id); //Use id string as id
			if (is_its_child_been_selected)
				ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.75f, 0.75f, 0.0f, 0.4f));

			bool isTreeOpen = ImGui::TreeNodeEx(idString.c_str(), nodeFlags, goName.c_str());

			if (is_its_child_been_selected)
				ImGui::PopStyleColor();
				

            ImGui::PopStyleColor();

			
			++selectedCounter; //counter here is for shift selecting
			
			//Standard Click and ctrl click
			if (ImGui::IsItemClicked(0)) {
				//Check if handle has been selected
				bool hasBeenSelected = false;
				int counter = 0;
				for (counter = 0; counter < selected_gameObjects.size(); ++counter) {
					if (handle == selected_gameObjects[counter]) {
						hasBeenSelected = true;
						break;
					}
				}

				if (ImGui::IsKeyDown(static_cast<int>(Key::Control))) { //Deselect that particular handle
					if (hasBeenSelected) {
						selected_gameObjects.erase(selected_gameObjects.begin() + counter);
					}
					else {
						selected_gameObjects.push_back(handle);
						Core::GetSystem<IDE>().command_controller.ExecuteCommand(COMMAND(CMD_SelectGameObject, handle));

					}
					hasSelected_GameobjectsModified = true;

				}
				else if (ImGui::IsKeyDown(static_cast<int>(Key::Shift))) {
					if (selected_gameObjects.size() != 0) {
						selectedItemCounter = selectedCounter;
						isShiftSelectCalled = true;


					}
				}
				
				else {
					//Select as normal
					selected_gameObjects.clear();
					selected_gameObjects.push_back(handle);
					Core::GetSystem<IDE>().command_controller.ExecuteCommand(COMMAND(CMD_SelectGameObject, handle));

					hasSelected_GameobjectsModified = true;
				}

				if (ImGui::IsMouseDoubleClicked(0)) {
					Core::GetSystem<IDE>().FocusOnSelectedGameObjects();
				}
			}
			
			//If the drag drops target on to the handle...
			if (ImGui::BeginDragDropTarget()) {
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(DragDrop::GAME_OBJECT)) {
					IM_ASSERT(payload->DataSize == sizeof(uint64_t));
					//uint64_t* source = static_cast<uint64_t*>(payload->Data); // Getting the Payload Data
					if (selected_gameObjects.size()) {
						for (Handle<GameObject>& i : selected_gameObjects) {
							if (i == handle) //do not parent self
								continue;

							Handle<GameObject> parentCheck = handle->GetComponent<Transform>()->parent; //Check for if im parenting to my own children
							bool isParentingToChild = false;
							while (parentCheck) {
								if (i == parentCheck) {
									isParentingToChild = true;
									break;
								}
								parentCheck = parentCheck->GetComponent<Transform>()->parent;
							}
							if (isParentingToChild) {
								continue;
							}

							//If im draging to my parent, unparent
							if (i->GetComponent<Transform>()->parent == handle) {
								i->GetComponent<Transform>()->parent = Handle <GameObject>{};
							}
							else { //Else parent normally
								i->GetComponent<Transform>()->SetParent(handle, true);
							}
						}
					}

				}
				ImGui::EndDragDropTarget();
			}
			if (ImGui::BeginDragDropSource()) //ImGuiDragDropFlags_
			{
				ImGui::SetDragDropPayload(DragDrop::GAME_OBJECT, &handle.id, sizeof(uint64_t)); // "STRING" is a tag! This is used in IGE_InspectorWindow
				ImGui::Text("Drag to another gameobject to parent.");
				ImGui::Text("Drag to parent to unparent.");
				ImGui::EndDragDropSource();
			}

			++total_gameobjects_displayed;
			if (scroll_focused_gameObject) {
				if (!is_scroll_focused_gameObject_found) {
					if (scroll_focused_gameObject == handle) {
						is_scroll_focused_gameObject_found = true;
					}
					else {
						++focused_gameobject_position;
					}
				}
			}


			if (isTreeOpen) {

				ImGui::TreePop();

			}
			else {
				itemToSkipInGraph.push_back(selectedCounter);
				return false; //Skip children
			}

			return true; //Go to next in visit. Does not skip children




		});

        ImGui::PopStyleVar();

		//Shift Select logic
		if (isShiftSelectCalled) {
			//std::cout << "Items to skip: ";
			//for (int& i : itemToSkipInGraph) {
			//	std::cout << i << ", ";
			//}
			//std::cout << std::endl;

			int counter = 0; //This is the same as the above scenegraph. I cant use the tree to track which places to skip, so we are using the vector
			vector<int> minMax{ -1,-1 };
			vector<Handle<GameObject>>& selected_gameObjects = Core::GetSystem<IDE>().selected_gameObjects;
			vector<Handle<GameObject>> selectedForSelect = selected_gameObjects;
			sceneGraph.visit([&](const Handle<GameObject>& handle, int depth) -> bool {

				depth;
				if (!handle) //Ignore handle zero
					return true;
				IDE& editor = Core::GetSystem<IDE>();
				Handle<Camera>& editorCam = editor.currentCamera().current_camera;
				if (handle == editorCam->GetGameObject())
					return true;
				++counter;

				//Finds what is selected and use as min and max
				for (int i = 0; i < selectedForSelect.size(); ++i) {
					if (selectedForSelect[i] == handle) {
						minMax[0] = minMax[0] == -1 ? counter : (minMax[0] > counter ? counter : minMax[0]);
						minMax[1] = minMax[1] == -1 ? counter : (minMax[1] < counter ? counter : minMax[1]);
						break;
					}
				}

				//Skips similar to closed trees
				for (int i = 0; i < itemToSkipInGraph.size(); ++i) {
					if (itemToSkipInGraph[i] == counter) {
						return false;
					}
				}


				return true;
			});
			//std::cout << "Seletected item: " << selectedItemCounter << std::endl;
			if (minMax[0] == minMax[1]) {
				if (minMax[0] < selectedItemCounter)
					minMax[1] = selectedItemCounter;
				else
					minMax[0] = selectedItemCounter;
			}
			else if (minMax[0] > selectedItemCounter) { //out of ranges, selectedItemCounter becomes min
				minMax[0] = selectedItemCounter;
			}
			else if (minMax[1] < selectedItemCounter) { //out of ranges, selectedItemCounter becomes max
				minMax[1] = selectedItemCounter;
			}

			selected_gameObjects.clear();
			counter = 0;
			int execute_counter = 0;
			sceneGraph.visit([&](const Handle<GameObject>& handle, int depth) -> bool {
				
				depth;
				
				if (!handle) //Ignore handle zero
					return true;

				++counter;



				if (counter >= minMax[0] && counter <= minMax[1]) {
					selected_gameObjects.push_back(handle);
					Core::GetSystem<IDE>().command_controller.ExecuteCommand(COMMAND(CMD_SelectGameObject, handle));
					++execute_counter;
					hasSelected_GameobjectsModified = true;
				}
				//Skips similar to closed trees
				for (int i = 0; i < itemToSkipInGraph.size(); ++i) {
					if (itemToSkipInGraph[i] == counter) {
						return false;
					}
				}
				return true;
			});

			Core::GetSystem<IDE>().command_controller.ExecuteCommand(COMMAND(CMD_CollateCommands, execute_counter));

			//std::cout << "MIN: " << minMax[0] << " MAX: " << minMax[1] << std::endl;

			//Refresh the new matrix values
		}
        if (hasSelected_GameobjectsModified)
        {
            Core::GetSystem<IDE>().RefreshSelectedMatrix();
            OnGameObjectSelectionChanged.Fire();
        }

		ImGui::PopStyleVar(); //ImGuiStyleVar_ItemSpacing


		if (scroll_focused_gameObject) { //If this was called by sceneview. scroll the hierarchy to this position then null this.

			if (is_scroll_focused_gameObject_found) {
				const int clamp_val = 5;
				if (focused_gameobject_position < clamp_val)
					focused_gameobject_position = 0;
				else if ((total_gameobjects_displayed-focused_gameobject_position) < clamp_val)
					focused_gameobject_position = total_gameobjects_displayed;
				const float pos = static_cast<float>(focused_gameobject_position) / static_cast<float>(total_gameobjects_displayed);
				ImGui::SetScrollY(ImGui::GetScrollMaxY() * pos); //sceneGraph will always be 1 or more
			}



			scroll_focused_gameObject = {};
		}


	}

	void IGE_HierarchyWindow::ScrollToSelectedInHierarchy(Handle<GameObject> gameObject)
	{
		//Find the position of the object in the hierarchy
		scroll_focused_gameObject = gameObject;
	}

	bool IGE_HierarchyWindow::CheckIfChildrenIsSelected(SceneManager::SceneGraph* childrenGraph, Handle<GameObject> comparingGameObject)
	{
		if (!childrenGraph)
			return false;
		if (childrenGraph->size() == 0)
			return false;

		bool is_child_selected = false;
		for (auto j = childrenGraph->begin(); j != childrenGraph->end(); ++j) {
			if ((*j).obj == comparingGameObject) {
				is_child_selected = true;
			}
			else {
				SceneManager& sceneManager = Core::GetSystem<SceneManager>();
				is_child_selected = CheckIfChildrenIsSelected(sceneManager.FetchSceneGraphFor((*j).obj), comparingGameObject);
			}

			if (is_child_selected)
				break;
		}

		return is_child_selected;


	}

}
