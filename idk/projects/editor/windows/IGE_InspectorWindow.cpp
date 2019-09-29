//////////////////////////////////////////////////////////////////////////////////
//@file		IGE_InspectorWindow.cpp
//@author	Muhammad Izha B Rahim
//@param	Email : izha95\@hotmail.com
//@date		28 SEPT 2019
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
#include <widgets/InputResource.h>
#include <meta/variant.h>

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



	void IGE_InspectorWindow::displayVal(reflect::dynamic dyn)
	{
		dyn.visit([&](auto&& key, auto&& val, int depth_change) { //Displays all the members for that variable

			using K = std::decay_t<decltype(key)>;
			using T = std::decay_t<decltype(val)>;

			//reflect::dynamic dynaVal = val;
			const float currentHeight = ImGui::GetCursorPosY();

			if constexpr (!std::is_same_v<K, const char*>)
				throw "Unhandled case";
			else
			{
				string key_str = key;

				if (key_str == "guid") {
					return false;
				}
				if (dyn.is<Animator>())
				{
					if (key_str == "_animation_table" ||
						key_str == "_animations" ||
						key_str == "_child_objects" ||
						key_str == "_bone_transforms" ||
						key_str == "_bind_pose")
						return false;
				}

				string keyName = key;
				keyName[0] = toupper(keyName[0]);
				ImGui::SetCursorPosY(currentHeight + heightOffset);
				ImGui::Text(keyName.c_str());
				keyName.insert(0, "##"); //For Imgui stuff


				ImGui::SameLine();
				ImGui::SetCursorPosY(currentHeight);


				if constexpr (is_sequential_container_v<T>)
				{
					reflect::uni_container cont{ val };
					ImGui::Button("+");
					ImGui::Indent();
					for (auto dyn : cont)
					{
						displayVal(dyn);
					}
					ImGui::Unindent();

					return false;
				}
				else if constexpr (is_associative_container_v<T>)
				{
					ImGui::Text("Associative Container");
					return false;
					/*reflect::uni_container cont{ val };
					ImGui::Button("+");
					ImGui::Indent();
					for (auto dyn : cont)
					{
						auto pair = dyn.unpack();
						displayVal(pair[0]);
					}
					ImGui::Unindent();*/
				}

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
				else if constexpr (is_template_v<T, RscHandle>) {

					if (ImGuidk::InputResource(keyName.c_str(), &val))
					{

					}

					return false;
				}
				else if constexpr (is_template_v<T, std::variant>)
				{
					static_assert(is_template_v<T, std::variant>, "HOW????");
					const int curr_ind = s_cast<int>(val.index());
					int new_ind = curr_ind;

					constexpr auto sz = reflect::detail::pack_size<T>::value; // THE FUUU?
					using VarCombo = std::array<const char*, sz>;


					static auto combo_items = []()-> VarCombo
					{
						static_assert(is_template_v<T, std::variant>, "HOW????");

						static std::array<string, sz> tmp_arr;
						std::array<const char*, sz> retval;

						auto sp = reflect::unpack_types<T>();

						for (auto i = 0; i < sz; ++i)
						{
							tmp_arr[i] = string{ sp[i].name() };
							retval[i] = tmp_arr[i].data();
						}
						return retval;
					}();

					if (ImGui::Combo(keyName.data(), &new_ind, combo_items.data(), std::size(combo_items)))
					{
						val = variant_construct<T>(new_ind);
					}
					return false;
				}
				else {
					ImGui::SetCursorPosY(currentHeight + heightOffset);
					ImGui::TextDisabled("Member type not defined in IGE_InspectorWindow::Update");
					return true;
				}
			}

		});
	}



	void IGE_InspectorWindow::Update()
	{
		
		ImGui::PopStyleVar(1);


		IDE& editor = Core::GetSystem<IDE>();
		const size_t gameObjectsCount = editor.selected_gameObjects.size();

		bool isComponentMarkedForDeletion = false;
		string componentNameMarkedForDeletion{}; //Is empty by default

		//DISPLAY
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
			for (auto& component : componentSpan) {

				//Skip Name and Transform
				if (component == c_name)
					continue;

				if (component == c_transform)
					continue;


				//COMPONENT DISPLAY
				ImGui::PushID(static_cast<int>(component.id));
				const auto componentName = (*component).type.name();
				string displayingComponent{ componentName };
				const string fluffText{ "idk::" };
				std::size_t found = displayingComponent.find(fluffText);

				if (found != std::string::npos)
					displayingComponent.erase(found, fluffText.size());

				ImVec2 cursorPos = ImGui::GetCursorPos();
				ImVec2 cursorPos2{}; //This is for setting after all members are placed
				ImGui::SetCursorPosX(window_size.x - 20);
				if (ImGui::ArrowButton("AdditionalOptions", ImGuiDir_Down)) { //This is hidden, so lets redraw this as text after the collapsing header.

					ImGui::OpenPopup("AdditionalOptions");

				}

				ImGui::SetCursorPos(cursorPos);

				if (ImGui::CollapsingHeader(displayingComponent.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
				{
					displayVal(*component);

					if (component.is_type<Animator>())
					{
						// ImGui::TextColored(ImVec4{ 1,0,0,1 }, "@IZAH/MAL: \n\tHelp me shift to correct place when free :3");
						if (ImGui::Button("Play"))
						{
							(*component).get<Animator>().Play(0);
						}
						if (ImGui::Button("Stop"))
						{
							(*component).get<Animator>().Stop();
						}
						if (ImGui::Button("Pause"))
						{
							(*component).get<Animator>().Pause();
						}
					}
				}

				cursorPos2 = ImGui::GetCursorPos();
				ImGui::SetCursorPos(cursorPos);
				ImGui::SetCursorPosX(window_size.x - 20);
				ImGui::Text("...");

				ImGui::SetCursorPos(cursorPos2);

				if (ImGui::BeginPopup("AdditionalOptions", ImGuiWindowFlags_None)) {
					if (ImGui::MenuItem("Reset")) {

					}
					ImGui::Separator();
					if (ImGui::MenuItem("Remove Component")) {
						isComponentMarkedForDeletion = true;
						componentNameMarkedForDeletion = (*component).type.name();
					}
					ImGui::EndPopup();
				}


				ImGui::PopID();
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

		//Add Component Button
		if (gameObjectsCount) {
			ImGui::Separator();
			ImGui::SetCursorPosX(window_size.x * 0.25f);
			if (ImGui::Button("Add Component", ImVec2{ window_size.x * 0.5f,0.0f })) {
				ImGui::OpenPopup("AddComp");
			}
		}

		if (isComponentMarkedForDeletion) {
			for (Handle<GameObject> i : editor.selected_gameObjects)
				editor.command_controller.ExecuteCommand(COMMAND(CMD_DeleteComponent, i, componentNameMarkedForDeletion));
		}

		if (ImGui::BeginPopup("AddComp", ImGuiWindowFlags_None)) {
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


				if (ImGui::MenuItem(displayName.c_str())) {
					//Add component
					for (Handle<GameObject> i : editor.selected_gameObjects)
						editor.command_controller.ExecuteCommand(COMMAND(CMD_AddComponent,i, string{ name }));
				}
			}
			ImGui::EndPopup();
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

		ImVec2 cursorPos = ImGui::GetCursorPos();
		ImVec2 cursorPos2{};
		ImGui::SetCursorPosX(window_size.x - 20);
		if (ImGui::ArrowButton("AdditionalOptions", ImGuiDir_Down)) { //This is hidden, so lets redraw this as text after the collapsing header.

			ImGui::OpenPopup("AdditionalOptions");

		}

		ImGui::SetCursorPos(cursorPos);


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

		cursorPos2 = ImGui::GetCursorPos();
		ImGui::SetCursorPos(cursorPos);
		ImGui::SetCursorPosX(window_size.x - 20);
		ImGui::Text("...");

		ImGui::SetCursorPos(cursorPos2);


		if (ImGui::BeginPopup("AdditionalOptions", ImGuiWindowFlags_None)) {
			if (ImGui::MenuItem("Reset")) {
				c_transform->position	= vec3{ };
				c_transform->rotation	= quat{ };
				c_transform->scale		= vec3{ };
			}
			ImGui::Separator();
			ImGui::EndPopup();
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
