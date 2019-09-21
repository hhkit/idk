//////////////////////////////////////////////////////////////////////////////////
//@file		IGE_InspectorWindow.cpp
//@author	Muhammad Izha B Rahim
//@param	Email : izha95\@hotmail.com
//@date		17 SEPT 2019
//@brief	

/*
This window controls the top main menu bar. It also controls the docking
of the editor.
*/
//////////////////////////////////////////////////////////////////////////////////



#include "pch.h"
#include <windows/IGE_InspectorWindow.h>
#include <editor/commands/CommandList.h>
#include <editorstatic/imgui/imgui_internal.h> //InputTextEx
#include <app/Application.h>
#include <scene/SceneManager.h>
#include <res/ResourceManager.h>
#include <reflect/reflect.h>
#include <IncludeComponents.h>
#include <IDE.h>
#include <iostream>
#include <ds/span.h>
#include <imgui/imgui_stl.h>
#include <math/euler_angles.h>

namespace idk {

	IGE_InspectorWindow::IGE_InspectorWindow()
		:IGE_IWindow{ "Inspector##IGE_InspectorWindow",true,ImVec2{ 300,600 },ImVec2{ 450,150 } } {		//Delegate Constructor to set window size
			// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.
		window_flags = ImGuiWindowFlags_NoCollapse;




	}

	void IGE_InspectorWindow::BeginWindow()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2{150.0f,100.0f});



	}
	void IGE_InspectorWindow::Update()
	{
		
		ImGui::PopStyleVar(1);


		IDE& editor = Core::GetSystem<IDE>();
		const size_t gameObjectsCount = editor.selected_gameObjects.size();
		if (gameObjectsCount == 1) {
			//Just show all components, Name and Transform first
			Handle<Name> c_name = editor.selected_gameObjects[0]->GetComponent<Name>();
			if (c_name) {
				DisplayNameComponent(c_name);

			}

			Handle<Transform> c_transform = editor.selected_gameObjects[0]->GetComponent<Transform>();
			if (c_transform) {
				DisplayTransformComponent(c_transform);
			}

			//Display remaining components here
			auto componentSpan = editor.selected_gameObjects[0]->GetComponents();
			if (componentSpan.size()) {
				for (auto& component : componentSpan) {
					if (component == c_name)
						continue;

					if (component == c_transform)
						continue;


					//COMPONENT DISPLAY
					ImGui::PushID(static_cast<int>(component.id));
					auto componentName = (*component).type.name();
					if (ImGui::CollapsingHeader(string(componentName).c_str(), ImGuiTreeNodeFlags_DefaultOpen))
					{
						(*component).visit([&](auto&& key, auto&& val, int depth_change) {
							using T = std::decay_t<decltype(val)>;
							reflect::dynamic dynaKey = std::forward<decltype(key)>(key);
							//reflect::dynamic dynaVal = val;
							const float currentHeight = ImGui::GetCursorPosY();
							string keyName = dynaKey.get<const char*>();

							if (keyName == "guid") {
								return false;
							}


							keyName[0] = toupper(keyName[0]);
							ImGui::SetCursorPosY(currentHeight + heightOffset);
							ImGui::Text(keyName.c_str());
							keyName.insert(0, "##"); //For Imgui stuff


							ImGui::SameLine();
							ImGui::SetCursorPosY(currentHeight);

							//ALL THE TYPE STATEMENTS HERE
							if constexpr (std::is_same_v<T, float> || std::is_same_v<T, real>) {
								
								ImGui::DragFloat(keyName.c_str(), &val);
							}
							else if constexpr (std::is_same_v<T, int>) {
								ImGui::DragInt(keyName.c_str(), &val);
							}
							
							else if constexpr (std::is_same_v<T, bool>) {
								ImGui::Checkbox(keyName.c_str(), &val);
							}
							else if constexpr (std::is_same_v<T, vec3>) {

								DisplayVec3(val);
								return false;
							}
							else if constexpr (std::is_same_v<T, RscHandle<Mesh>>) {
								string meshName{val.guid };
								
								if (ImGui::Button(meshName.c_str())) {

								}
								//Create a drag drop payload on selected gameobjects.
								if (ImGui::BeginDragDropTarget()) {
									if (const ImGuiPayload * payload = ImGui::AcceptDragDropPayload("string")) {
										IM_ASSERT(payload->DataSize == sizeof(string));
										PathHandle* source = static_cast<PathHandle*>(payload->Data); // Getting the Payload Data
										if (source->GetExtension() != ".fbx") {

											
											//PathHandle PathHandle{ source->data() };
											//auto file = Core::GetSystem<ResourceManager>().GetFileResources(PathHandle);
											//file.resources[0].visit([&](auto& handle) {
											//	if constexpr (std::is_same_v < std::decay_t<decltype(handle)>, RscHandle<Mesh>>)
											//	{
											//		const RscHandle<Mesh>& mesh_handle = handle;
											//		val = mesh_handle;
											//	}
											//	else
											//		(handle);
											//});



										}

									}
									ImGui::EndDragDropTarget();
								}


								return false;
							}
							else {
								ImGui::SetCursorPosY(currentHeight + heightOffset);
								ImGui::TextDisabled("Member type not defined in IGE_InspectorWindow::Update");
							}
							
						});


					}
					ImGui::PopID();
				}




			}



		}
		else if (gameObjectsCount > 1) {
			//Just show similar components

			//for (auto& i : editor.selected_gameObjects) {
			//
			//
			//	//Hierarchy Display
			//	SceneManager& sceneManager = Core::GetSystem<SceneManager>();
			//	SceneManager::SceneGraph& sceneGraph = sceneManager.FetchSceneGraph();
			//	sceneGraph.visit([](const Handle<GameObject>& handle, int depth) {
			//
			//
			//
			//		});
			//
			//}

		}







	}

	void IGE_InspectorWindow::DisplayNameComponent(Handle<Name>& c_name)
	{
		static string stringBuf{};
		IDE& editor = Core::GetSystem<IDE>();

		Handle<GameObject> gameObject = c_name->GetGameObject();
		ImGui::Text("Name: ");
		ImGui::SameLine();
		if (ImGui::InputText("##Name", &stringBuf, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_NoUndoRedo)) {
			//c_name->name = stringBuf;
			editor.command_controller.ExecuteCommand(COMMAND(CMD_ModifyInput<string>, GenericHandle{ c_name }, &c_name->name, stringBuf));


		}
		//if (ImGui::IsItemDeactivatedAfterEdit()) {
		//}

		if (ImGui::IsItemClicked()) {
			stringBuf = c_name->name;
		}
		else if (!ImGui::IsItemActive()) { //Disable assignment when editing text
			stringBuf = c_name->name;
		}



		
		string idName = std::to_string(gameObject.id);
		ImGui::Text("ID: %s", idName.data());
		//ImGui::SameLine();
		//ImGui::InputText("##ID", &idName, ImGuiInputTextFlags_ReadOnly);
		//if (ImGui::InputText("##NAME", &selectedGameObject.lock()->name, ImGuiInputTextFlags_EnterReturnsTrue)) {
	}

	void IGE_InspectorWindow::DisplayTransformComponent(Handle<Transform>& c_transform)
	{
		if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
		{


			//Position
			float heightPos = ImGui::GetCursorPosY();
			ImGui::SetCursorPosY(heightPos + heightOffset);
			ImGui::Text("Position");
			ImGui::SameLine();

			DisplayVec3(c_transform->position);

			//Rotation (use custom vec3 display)
			euler_angles original{ c_transform->rotation };


			heightPos = ImGui::GetCursorPosY();
			ImGui::SetCursorPosY(heightPos + heightOffset);
			ImGui::Text("Rotation");
			ImGui::SameLine();
			ImGui::PushItemWidth(window_size.x * float3Size - itemSpacing);
			ImGui::SetCursorPosX(widthOffset);

			ImGui::Text("X");
			ImGui::SameLine();
			ImGui::SetCursorPosY(heightPos);
			if (ImGui::SliderAngle("##RotationX", original.x.data())) {
				c_transform->rotation = quat{ original };
			}

			ImGui::SameLine();

			ImGui::Text("Y");
			ImGui::SameLine();
			if (ImGui::SliderAngle("##RotationY", original.y.data())) {
				c_transform->rotation = quat{ original };
			}

			ImGui::SameLine();

			ImGui::Text("Z");
			ImGui::SameLine();
			if (ImGui::SliderAngle("##RotationZ", original.z.data())) {
				c_transform->rotation = quat{ original };
			}

			ImGui::PopItemWidth();

			//Scale
			heightPos = ImGui::GetCursorPosY();
			ImGui::SetCursorPosY(heightPos + heightOffset);
			ImGui::Text("Scale");
			ImGui::SameLine();

			DisplayVec3(c_transform->scale);




		}
	}

	void IGE_InspectorWindow::DisplayVec3(vec3& vec)
	{
		ImGui::PushItemWidth(window_size.x * float3Size - itemSpacing);
		ImGui::SetCursorPosX(widthOffset);

		ImGui::Text("X");
		ImGui::SameLine();

		ImGui::SetCursorPosY(ImGui::GetCursorPosY() - heightOffset);
		ImGui::PushID(&vec.x);
		ImGui::DragFloat("##X", &vec.x);
		ImGui::PopID();
		ImGui::SameLine();

		ImGui::Text("Y");
		ImGui::SameLine();
		ImGui::PushID(&vec.y);
		ImGui::DragFloat("##Y", &vec.y);
		ImGui::PopID();
		ImGui::SameLine();

		ImGui::Text("Z");
		ImGui::SameLine();
		ImGui::PushID(&vec.z);
		ImGui::DragFloat("##Z", &vec.z);
		ImGui::PopID();

		ImGui::PopItemWidth();

	}


}
