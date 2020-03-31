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
#include <scene/SceneGraph.inl>
#include <script/ScriptSystem.h>
#include <script/MonoBehavior.h>


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
		auto& editor = Core::GetSystem<IDE>();

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
					parent = parent->Parent();
				}
				if (!parent)
				{
					if (ImGui::MenuItem("Canvas"))
						editor.CreateGameObject(go, "Canvas", vector<string>{ "RectTransform", "Canvas" });
				}
				if (ImGui::MenuItem("Image"))
					editor.CreateGameObject(go, "Image", vector<string>{ "RectTransform", "Image" });
				if (ImGui::MenuItem("Text"))
					editor.CreateGameObject(go, "Text", vector<string>{ "RectTransform", "Text" });
				if (ImGui::MenuItem("AspectRatioFitter"))
					editor.CreateGameObject(go, "AspectRatioFitter", vector<string>{ "RectTransform", "AspectRatioFitter" });
				
				ImGui::EndMenu();
            }

			ImGui::EndPopup();
		}

		if (ImGui::Button("...##AdditionalStuff")) {
			ImGui::OpenPopup("OpenAdditionalStuff");
		}
		if (ImGui::BeginPopup("OpenAdditionalStuff", ImGuiWindowFlags_NoMove)) {
			ImGui::Checkbox("Show Editor Objects", &show_editor_objects);
			ImGui::Checkbox("Component Finder Mode", &component_finder_mode);
			ImGui::EndPopup();
		}
		
		if (ImGui::BeginPopup("ComponentFinder", ImGuiWindowFlags_None)) {
			ImGui::Text("Search bar:");
			ImGui::SameLine();
			const bool value_changed2 = ImGui::InputTextEx("##hierarchyComp_textFilter", NULL, component_textFilter.InputBuf, IM_ARRAYSIZE(component_textFilter.InputBuf), ImVec2{ 100,0 }, ImGuiInputTextFlags_None);
			if (value_changed2)
				component_textFilter.Build();
			
			ImGui::Separator();
			
			if (ImGui::MenuItem("Cancel Selection"))
			{
				component_finder_text = "";
			}
			ImGui::Separator();
			ImGui::TextColored(ImVec4{0,1,0,1},"COMPONENTS");
			ImGui::Separator();
			
			span componentNames = GameState::GetComponentNames();
			for (const char* name : componentNames)
			{
				string displayName = name;
				if (displayName == "Transform" ||
					displayName == "Name" ||
					displayName == "Tag" ||
					displayName == "Layer" ||
					displayName == "PrefabInstance" ||
					displayName == "MonoBehavior")
					continue;
			
				//Comment/Uncomment this to remove text fluff 
				const string fluffText{ "idk::" };
			
				std::size_t found = displayName.find(fluffText);
				if (found != std::string::npos)
					displayName.erase(found, fluffText.size());
			
				if (!component_textFilter.PassFilter(displayName.c_str())) //skip if filtered
					continue;
			
				if (ImGui::MenuItem(displayName.c_str()))
				{
					component_finder_text = name;
					isComponentAScript = false;
				}
			}

			ImGui::Separator();
			ImGui::TextColored(ImVec4{ 0,1,0,1 }, "SCRIPTS");
			ImGui::Separator();
			
			auto* script_env = &Core::GetSystem<mono::ScriptSystem>().ScriptEnvironment();
			if (script_env == nullptr)
				ImGui::Text("Scripts not loaded!");
			else {

				span scriptNames = script_env->GetBehaviorList();
				for (const char* name : scriptNames)
				{
					if (!component_textFilter.PassFilter(name)) //skip if filtered
						continue;

					if (ImGui::MenuItem(name))
					{
						component_finder_text = name;
						isComponentAScript = true;
					}
				}
			}
			ImGui::EndPopup();
		}
		
		ImGui::SameLine();
		ImVec2 startPos = ImGui::GetCursorPos();
		ImGui::SetCursorPosX(startPos.x+15);

		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 12.0f);
		const auto search_bar_offset_x = ImGui::GetCursorPosX() + 20;
		//Can call textFilter.Draw(), but I want to use a custom inputText.
		if (component_finder_mode) {
			if (component_finder_text.empty()) {
				if (ImGui::Button("Search Component", ImVec2{ window_size.x * 0.5f,0.0f })) {
					ImGui::OpenPopup("ComponentFinder");
				}
			}
			else {
				if (ImGui::Button(component_finder_text.c_str(), ImVec2{ window_size.x * 0.5f,0.0f })) {
					ImGui::OpenPopup("ComponentFinder");
				}
			}
			
		}
		else {
			const bool value_changed = ImGui::InputTextEx("##ToolBarSearchBar", NULL, textFilter.InputBuf, IM_ARRAYSIZE(textFilter.InputBuf), ImVec2{ window_size.x - search_bar_offset_x,ImGui::GetFrameHeight() - 2 }, ImGuiInputTextFlags_None);
			if (value_changed)
				textFilter.Build();

			if (!ImGui::IsItemActive())
				DrawToolTipOnHover("Type here to filter gameobjects.\n" "Add a '-' to exclude gameobjects.");
		}

		ImGui::PopStyleVar();

		ImGui::EndMenuBar();
		// ================================== menu bar


		//Hierarchy Display
		SceneManager& sceneManager = Core::GetSystem<SceneManager>();
		auto sceneGraph = sceneManager.FetchSceneGraph();
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0.0f,0.0f });
		//ImGui::Checkbox("Show Editor Objs", &show_editor_objects);
		
		//To unindent the first gameobject which is the scene
		ImGui::Unindent();

		Handle<GameObject> scroll_focus_next_frame;
		bool shift_selecting = false;
		ObjectSelection selection = editor.GetSelectedObjects();
		vector<Handle<GameObject>>& selected_gameObjects = selection.game_objects;

		if (shift_select_anchors[0] && shift_select_anchors[1])
			selected_gameObjects.clear();

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4.0f, 1.0f));

		//Display gameobjects
		sceneGraph.Visit([&](const Handle<GameObject> handle, int depth) -> bool 
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
			auto children = sceneManager.FetchSceneGraphFor(handle);
			if (children.GetNumChildren() == 0)
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
			
			if (component_finder_mode) {
				if (!component_finder_text.empty()) {
					if (isComponentAScript) {
						auto monobehaviorHandle = handle->GetComponent("MonoBehavior");
						if (!monobehaviorHandle) {
							ImGui::PopStyleColor();
							return true;
						}
						auto mb_handle = handle_cast<mono::Behavior>(monobehaviorHandle);
						if (!mb_handle) {
							ImGui::PopStyleColor();
							return true;
						}
						if (mb_handle->TypeName() != component_finder_text) {
							ImGui::PopStyleColor();
							return true;
						}
					}
					else {
						auto componentHandle = handle->GetComponent(component_finder_text);
						if (!componentHandle) {
							ImGui::PopStyleColor();
							return true;
						}
					}
				}
			}
			else if (!textFilter.PassFilter(goName.c_str())) {
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
						editor.SetSelection(selection);
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
						editor.SetSelection(selection);
					}
					else {
						editor.SelectGameObject(handle, true);
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
					editor.SelectGameObject(handle);
					shift_select_anchors[0] = handle;
				}
			}

			if (ImGui::IsMouseDoubleClicked(0) && ImGui::IsItemHovered())
			{
				editor.FocusOnSelectedGameObjects();
			}

			//If the drag drops target on to the handle...
			if (ImGui::BeginDragDropTarget()) 
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(DragDrop::GAME_OBJECT)) 
				{
					IM_ASSERT(payload->DataSize == sizeof(uint64_t));
					Handle<GameObject> drop_payload = Handle<GameObject>{ *static_cast<uint64_t*>(payload->Data) }; // Getting the Payload Data
					auto object_itr = std::find(selected_gameObjects.begin(), selected_gameObjects.end(), drop_payload);

					int execute_counter = 0;
					if (object_itr != selected_gameObjects.end()) // object is amongst the multi selected
					{
						for (Handle<GameObject> i : selected_gameObjects)
						{
							if (!i || i == handle) //do not parent self
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
								editor.ExecuteCommand<CMD_ParentGameObject>(i, Handle<GameObject>{});
							else //Else parent normally
								editor.ExecuteCommand<CMD_ParentGameObject>(i, handle);
							++execute_counter;
						}

						editor.ExecuteCommand<CMD_CollateCommands>(execute_counter);
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
								editor.ExecuteCommand<CMD_ParentGameObject>(i, Handle<GameObject>{});
							else //Else parent normally
								editor.ExecuteCommand<CMD_ParentGameObject>(i, handle);
							editor.SelectGameObject(i, false, true);
							editor.ExecuteCommand<CMD_CollateCommands>(2);
						}
					}

					scroll_focus_next_frame = drop_payload;
				}
				ImGui::EndDragDropTarget();
			}
			if (ImGui::BeginDragDropSource()) //ImGuiDragDropFlags_
			{
				ImGui::SetDragDropPayload(DragDrop::GAME_OBJECT, &handle.id, sizeof(uint64_t));
				ImGui::Text("Dragging gameobject: ");
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
				{
					ImGui::SetScrollHereY();
					scroll_focused_gameObject = {};
				}
			}


			if (isTreeOpen)
				ImGui::TreePop();
			else
				return false; //Skip children

			return true; //Go to next in visit. Does not skip children
		});

		ImGui::PopStyleVar(2); // ImGuiStyleVar_ItemSpacing, FramePadding

		if (scroll_focus_next_frame)
			ScrollToSelectedInHierarchy(scroll_focus_next_frame);

		if (ImGui::InvisibleButton("empty_space", ImGui::GetContentRegionAvail()))
		{
			editor.Unselect();
		}
		if (ImGui::BeginDragDropTarget()) // drag onto empty space unparents gameobjects
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(DragDrop::GAME_OBJECT, ImGuiDragDropFlags_AcceptNoDrawDefaultRect))
			{
				IM_ASSERT(payload->DataSize == sizeof(uint64_t));
				Handle<GameObject> drop_payload = Handle<GameObject>{ *static_cast<uint64_t*>(payload->Data) }; // Getting the Payload Data
				auto object_itr = std::find(selected_gameObjects.begin(), selected_gameObjects.end(), drop_payload);

				int execute_counter = 0;
				if (object_itr != selected_gameObjects.end()) // object is amongst the multi selected
				{
					for (auto h : selected_gameObjects)
					{
						if (!h)
							continue;
						editor.ExecuteCommand<CMD_ParentGameObject>(h, Handle<GameObject>{});
						++execute_counter;
					}
					editor.ExecuteCommand<CMD_CollateCommands>(execute_counter);
				}
				else
				{
					editor.ExecuteCommand<CMD_ParentGameObject>(drop_payload, Handle<GameObject>{});
					editor.SelectGameObject(drop_payload, false, true);
					editor.ExecuteCommand<CMD_CollateCommands>(2);
				}
			}
		}

		if (ImGui::IsWindowFocused() && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Delete)))
			editor.DeleteSelectedGameObjects();
	}

	void IGE_HierarchyWindow::ScrollToSelectedInHierarchy(Handle<GameObject> gameObject)
	{
		//Find the position of the object in the hierarchy
		scroll_focused_gameObject = gameObject;
	}

	bool IGE_HierarchyWindow::CheckIfChildrenIsSelected(SceneGraphHandle childrenGraph, Handle<GameObject> comparingGameObject)
	{
		if (!childrenGraph)
			return false;
		if (childrenGraph.GetNumChildren() == 0)
			return false;

		bool is_child_selected = false;
		for (auto j = childrenGraph.begin(); j != childrenGraph.end(); ++j) {
			if (*j == comparingGameObject) {
				is_child_selected = true;
			}
			else {
				SceneManager& sceneManager = Core::GetSystem<SceneManager>();
				is_child_selected = CheckIfChildrenIsSelected(sceneManager.FetchSceneGraphFor(*j), comparingGameObject);
			}

			if (is_child_selected)
				break;
		}

		return is_child_selected;


	}

}
