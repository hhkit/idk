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
#include <core/GameObject.inl>
#include <common/Name.h>
#include <common/Transform.h>
#include <prefab/PrefabInstance.h>
#include <gfx/Camera.h>
#include <core/Core.h>
#include <prefab/PrefabUtility.h>
#include <serialize/text.inl>
#include <IDE.h>		//IDE
#include <iostream>
#include <res/ResourceHandle.inl>

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

		// MenuBar

        ImGui::BeginMenuBar();
		ImGui::PopStyleColor();
		ImGui::PopStyleVar(3);

		ImGui::SetCursorPosX(5);
		if (ImGui::Button("Create")) 
		{
			ImGui::OpenPopup("CreatePopup");
		}
		if (ImGui::BeginPopup("CreatePopup")) 
		{
            IDE& editor = Core::GetSystem<IDE>();

			if (ImGui::MenuItem("Create Empty", "CTRL+SHIFT+N"))
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
            if (ImGui::BeginMenu("UI")) 
			{
				const auto go = editor.GetSelectedObjects().game_objects.empty() ?
					Handle<GameObject>() : editor.GetSelectedObjects().game_objects.front();

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
						editor.CreateGameObject(go, "Canvas", vector<string>{ "RectTransform", "Canvas" });
					}
				}
				if (ImGui::MenuItem("Image"))
					editor.CreateGameObject(go, "Image", vector<string>{ "RectTransform", "Image" });
				if (ImGui::MenuItem("Text"))
					editor.CreateGameObject(go, "Text", vector<string>{ "RectTransform", "Text" });
				
				ImGui::EndMenu();
            }

			ImGui::EndPopup();
		}

		if (ImGui::Button("...##AdditionalStuff")) {
			ImGui::OpenPopup("OpenAdditionalStuff");
		}
		if (ImGui::BeginPopup("OpenAdditionalStuff", ImGuiWindowFlags_NoMove)) {
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
		// ================================== menu bar


		//Hierarchy Display
		SceneManager& sceneManager = Core::GetSystem<SceneManager>();
		SceneManager::SceneGraph& sceneGraph = sceneManager.FetchSceneGraph();
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0.0f,0.0f });
		//ImGui::Checkbox("Show Editor Objs", &show_editor_objects);
		
		//To unindent the first gameobject which is the scene
		ImGui::Unindent();

		bool shift_selecting = false;

		ObjectSelection selection = Core::GetSystem<IDE>().GetSelectedObjects();
		vector<Handle<GameObject>>& selected_gameObjects = selection.game_objects;

		if (shift_select_anchors[0] && shift_select_anchors[1])
			selected_gameObjects.clear();

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4.0f, 1.0f));

		//Display gameobjects
		sceneGraph.visit([&](const Handle<GameObject> handle, int depth) -> bool 
		{
			if (depth > 0) {
				for (int i = 0; i < depth; ++i)
					ImGui::Indent();
			}
			else {
				for (int i = depth; i < 0; ++i)
					ImGui::Unindent();
			}

			if (!handle) //Ignore null/invalid handle
				return true;
			if (!show_editor_objects && handle.scene == Scene::editor || handle.scene == Scene::prefab) // ignore editor
				return true;

			ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_SpanFullWidth;

			SceneManager& sceneManager = Core::GetSystem<SceneManager>();
			SceneManager::SceneGraph* children = sceneManager.FetchSceneGraphFor(handle);
			if (children->size() == 0)
				nodeFlags |= ImGuiTreeNodeFlags_Leaf;

			bool is_its_child_been_selected = false;
			//Check if gameobject has been selected. Causes Big-O(n^2)
			for (auto i : selected_gameObjects) 
			{
				if (handle == i) 
				{
					nodeFlags |= ImGuiTreeNodeFlags_Selected;
					break;
				}
				else if (CheckIfChildrenIsSelected(children, i))
				{
					is_its_child_been_selected = true;
					nodeFlags |= ImGuiTreeNodeFlags_Selected;
				}
			}

			Handle<Name> c_name = handle->GetComponent<Name>();
			string goName{};
			if (c_name)
				goName = c_name->name;
			
            const bool is_prefab = handle->HasComponent<PrefabInstance>();
            ImColor col = ImGui::GetColorU32(ImGuiCol_Text);
			if (goName.empty())
			{
				goName = "Unnamed (";
				goName.append(serialize_text(handle.id));
				goName.append(")");
                col = ImVec4(0.75f, 0.75f, 0.75f, 1.0f);
			}
            if (is_prefab)
                col = handle->GetComponent<PrefabInstance>()->prefab ? style.Colors[ImGuiCol_PlotLinesHovered] : ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
            if (!handle->ActiveInHierarchy())
                col.Value.w = 0.5f;

            ImGui::PushStyleColor(ImGuiCol_Text, col.Value);
			
			if (!textFilter.PassFilter(goName.c_str())) {
                ImGui::PopStyleColor();
				return true;
			}
			
			string idString = std::to_string(handle.id); //Use id string as id
			if (is_its_child_been_selected)
			{
				ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.75f, 0.75f, 0.0f, 0.4f));
				if (scroll_focused_gameObject)
					ImGui::SetNextItemOpen(true); // open this tree node to reveal the child
			}

			//Draw Node. Trees will always return true if open, so use IsItemClicked to set object instead!
			bool isTreeOpen = ImGui::TreeNodeEx(idString.c_str(), nodeFlags, goName.c_str());

			if (is_its_child_been_selected)
				ImGui::PopStyleColor();

            ImGui::PopStyleColor();

			// handle shift selecting here.
			// [0] is the start anchor, [1] is what the user shift selected to
			if (shift_select_anchors[0] && shift_select_anchors[1])
			{
				if (!shift_selecting)
				{
					if (handle == shift_select_anchors[0] || handle == shift_select_anchors[1])
					{
						shift_selecting = true;
						selected_gameObjects.push_back(handle);
					}
				}
				else // shift_selecting
				{
					selected_gameObjects.push_back(handle);
					if (handle == shift_select_anchors[0] || handle == shift_select_anchors[1])
					{
						shift_selecting = false;
						Core::GetSystem<IDE>().SetSelection(selection);
						shift_select_anchors[1] = {}; // keep the start anchor which is [0]
					}
				}
			}
			
			//Standard Click and ctrl click
			if (ImGui::IsMouseReleased(0) && ImGui::IsItemHovered() && !ImGui::IsItemToggledOpen())
			{
				if (ImGui::GetIO().KeyCtrl) //Deselect that particular handle
				{
					//Check if handle has been selected
					bool hasBeenSelected = false;
					int counter = 0;
					for (counter = 0; counter < selected_gameObjects.size(); ++counter) {
						if (handle == selected_gameObjects[counter]) {
							hasBeenSelected = true;
							break;
						}
					}

					if (hasBeenSelected) {
						selected_gameObjects.erase(selected_gameObjects.begin() + counter);
						Core::GetSystem<IDE>().SetSelection(selection);
					}
					else {
						Core::GetSystem<IDE>().SelectGameObject(handle, true);
					}

					shift_select_anchors[0] = handle;
				}
				else if (ImGui::GetIO().KeyShift)
				{
					if (shift_select_anchors[0])
						shift_select_anchors[1] = handle;
				}
				else
				{
					Core::GetSystem<IDE>().SelectGameObject(handle);
					shift_select_anchors[0] = handle;
				}

				if (ImGui::IsMouseDoubleClicked(0)) 
				{
					Core::GetSystem<IDE>().FocusOnSelectedGameObjects();
				}
			}

			//If the drag drops target on to the handle...
			if (ImGui::BeginDragDropTarget()) 
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(DragDrop::GAME_OBJECT)) {
					IM_ASSERT(payload->DataSize == sizeof(uint64_t));
					Handle<GameObject> drop_payload = Handle<GameObject>{ *static_cast<uint64_t*>(payload->Data) }; // Getting the Payload Data
					auto object_itr = std::find(selected_gameObjects.begin(), selected_gameObjects.end(), drop_payload);

					if (object_itr != selected_gameObjects.end()) // object is amongst the multi selected
					{
						for (Handle<GameObject>& i : selected_gameObjects)
						{
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
							if (i->Parent() == handle)
								Core::GetSystem<IDE>().command_controller.ExecuteCommand(COMMAND(CMD_ParentGameObject, i, Handle<GameObject>{}));
							else //Else parent normally
								Core::GetSystem<IDE>().command_controller.ExecuteCommand(COMMAND(CMD_ParentGameObject, i, handle));
						}

						Core::GetSystem<IDE>().command_controller.ExecuteCommand(COMMAND(CMD_CollateCommands, static_cast<int>(selected_gameObjects.size())));
					}
					else
					{
						auto i = drop_payload;
						Handle<GameObject> parentCheck = handle->GetComponent<Transform>()->parent; //Check for if im parenting to my own children
						bool isParentingToChild = false;
						while (parentCheck) {
							if (i == parentCheck) {
								isParentingToChild = true;
								break;
							}
							parentCheck = parentCheck->GetComponent<Transform>()->parent;
						}
						if (!isParentingToChild)
						{
							if (i->Parent() == handle)
								Core::GetSystem<IDE>().command_controller.ExecuteCommand(COMMAND(CMD_ParentGameObject, i, Handle<GameObject>{}));
							else //Else parent normally
								Core::GetSystem<IDE>().command_controller.ExecuteCommand(COMMAND(CMD_ParentGameObject, i, handle));
						}
					}

				}
				ImGui::EndDragDropTarget();
			}
			if (ImGui::BeginDragDropSource()) //ImGuiDragDropFlags_
			{
				ImGui::SetDragDropPayload(DragDrop::GAME_OBJECT, &handle.id, sizeof(uint64_t)); // "STRING" is a tag! This is used in IGE_InspectorWindow
				ImGui::Text("Draging gameobject: ");
				ImGui::SameLine();
				ImGui::TextColored(ImVec4(1,1,0,1),goName.c_str());
				ImGui::Text("Drag to another gameobject to parent.");
				ImGui::Text("Drag to parent to unparent.");
				ImGui::Text("Drag ProjectContent to create Prefab.");
				ImGui::EndDragDropSource();
			}


			if (scroll_focused_gameObject)
			{
				if (scroll_focused_gameObject == handle)
					ImGui::SetScrollHereY();
			}


			if (isTreeOpen)
				ImGui::TreePop();
			else
				return false; //Skip children

			return true; //Go to next in visit. Does not skip children
		});

		ImGui::PopStyleVar(2); // ImGuiStyleVar_ItemSpacing, FramePadding

		// reset this, scrolled to in visit
		scroll_focused_gameObject = {};

		if (ImGui::InvisibleButton("empty_space", ImGui::GetContentRegionAvail()))
		{
			Core::GetSystem<IDE>().Unselect();
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
