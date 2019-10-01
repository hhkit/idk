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



	static string format_name(string_view name)
	{
		string str{ name };
		str[0] = (char)toupper(str[0]);
		for (int i = 0; i < str.size(); ++i)
		{
			if (str[i] == '_')
			{
				str[i] = ' ';
				if (i + 1 < str.size())
					str[i + 1] = (char)toupper(str[i + 1]);
			}
		}
		for (int i = 1; i < str.size(); ++i)
		{
			if (str[i] >= 'A' && str[i] <= 'Z' && str[i - 1] >= 'a' && str[i - 1] <= 'z')
			{
				str.insert(i, 1, ' ');
			}
		}
		return str;
	}

	void IGE_InspectorWindow::displayVal(reflect::dynamic dyn)
	{
		dyn.visit([&](auto&& key, auto&& val, int /*depth_change*/) { //Displays all the members for that variable

			using K = std::decay_t<decltype(key)>;
			using T = std::decay_t<decltype(val)>;

			//reflect::dynamic dynaVal = val;
			const float currentHeight = ImGui::GetCursorPosY();

			if constexpr (std::is_same_v<K, reflect::type>) // from variant visit
				return true;
			else if constexpr (!std::is_same_v<K, const char*>)
				throw "Unhandled case";
			else
			{
				string keyName = format_name(key);

				if (keyName == "Guid")
					return false;

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
				else if constexpr (std::is_same_v<T, float> || std::is_same_v<T, real>) {
					ImGui::DragFloat(keyName.c_str(), &val);
					return false;
				}
				else if constexpr (std::is_same_v<T, int>) {
					ImGui::DragInt(keyName.c_str(), &val);
					return false;
				}

				else if constexpr (std::is_same_v<T, bool>) {
					ImGui::Checkbox(keyName.c_str(), &val);
					return false;
				}
				else if constexpr (std::is_same_v<T, vec3>) {
					DisplayVec3(val);
					return false;
				}
				else if constexpr (std::is_same_v<T, color>)
				{
					ImGui::ColorEdit4(keyName.c_str(), val.data());
					return false;
				}
				else if constexpr (std::is_same_v<T, rad>)
				{
					ImGui::SliderAngle(keyName.c_str(), val.data());
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

						constexpr auto sz = reflect::detail::pack_size<T>::value; // THE FUUU?
						static std::array<string, sz> tmp_arr;
						std::array<const char*, sz> retval;

						auto sp = reflect::unpack_types<T>();

						for (auto i = 0; i < sz; ++i)
						{
							tmp_arr[i] = format_name(sp[i].name());
							retval[i] = tmp_arr[i].data();
						}
						return retval;
					}();

					if (ImGui::Combo(keyName.data(), &new_ind, combo_items.data(), std::size(combo_items)))
					{
						val = variant_construct<T>(new_ind);
					}

					return true;
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


		//DISPLAY
		if (gameObjectsCount == 1) {
			//Just show all components, Name and Transform first
			Handle<Name> c_name = editor.selected_gameObjects[0]->GetComponent<Name>();
			if (c_name) {
				DisplayNameComponent(c_name);
			}

            if (editor.selected_gameObjects[0]->HasComponent<PrefabInstance>())
                DisplayPrefabInstanceControls(editor.selected_gameObjects[0]->GetComponent<PrefabInstance>());

			Handle<Transform> c_transform = editor.selected_gameObjects[0]->GetComponent<Transform>();
			if (c_transform) {
				DisplayTransformComponent(c_transform);
			}

			//Display remaining components here
			auto componentSpan = editor.selected_gameObjects[0]->GetComponents();
			for (auto& component : componentSpan) {

				//Skip Name and Transform and PrefabInstance
				if (component == c_name || component == c_transform || component.is_type<PrefabInstance>())
					continue;

				if (component.is_type<Animator>())
				{
					Handle<Animator> c_anim = editor.selected_gameObjects[0]->GetComponent<Animator>();
					DisplayAnimatorComponent(c_anim);
					continue;
				}

				//COMPONENT DISPLAY
				DisplayOtherComponent(component);
			}

		}
		else if (gameObjectsCount > 1) {

			//Just show all components, Name and Transform first
			//First gameobject takes priority. By default, name and transform will always be shown.
			Handle<Name> c_name = editor.selected_gameObjects[0]->GetComponent<Name>();
			if (c_name) {
				DisplayNameComponent(c_name);

			}
			Handle<Transform> c_transform = editor.selected_gameObjects[0]->GetComponent<Transform>();
			if (c_transform) {
				DisplayTransformComponent(c_transform);
			}


			//Just show similar components, based on first object
			span<GenericHandle> componentSpan = editor.selected_gameObjects[0]->GetComponents();
			hash_set<string,std::hash<string>,std::equal_to<string>> similarComponentNames;
			for (GenericHandle component : componentSpan) {
				if (component == c_name)
					continue;
				if (component == c_transform)
					continue;
				similarComponentNames.insert(string((*component).type.name()));
			}


			//Loop similarComponent
			//Inside loop, for each selected check if it contains such handle
			//If hit one, go to next similarComponent
			//If not, remove!
			vector<string> componentsToDiscard{};
			for (string componentName : similarComponentNames) {
				bool doesAllSelectedHasComponent = true;
				for (auto& gameObject : editor.selected_gameObjects) {
					if (!gameObject->GetComponent(componentName)) {
						doesAllSelectedHasComponent = false;
						break;
					}
				}
				if (!doesAllSelectedHasComponent) {
					componentsToDiscard.push_back(componentName);
				}
			}

			for (string i : componentsToDiscard) {
				similarComponentNames.erase(i);
			}

			//At this point we are sure that these components have these names
			for (string i : similarComponentNames) {
				GenericHandle component = editor.selected_gameObjects[0]->GetComponent(i);
				DisplayOtherComponent(component);
				//ImGui::Text(i.c_str());
			}

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

			//Reset values
			componentNameMarkedForDeletion = {};
			isComponentMarkedForDeletion = false;

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
		//The c_name is to just get the first gameobject
		static string stringBuf{};
		IDE& editor = Core::GetSystem<IDE>();
		//ImVec2 startScreenPos = ImGui::GetCursorScreenPos();
		Handle<GameObject> gameObject = c_name->GetGameObject();
		ImGui::Text("Name: ");
		ImGui::SameLine();
		if (ImGui::InputText("##Name", &stringBuf, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_NoUndoRedo)) {
			//c_name->name = stringBuf;
			for (size_t i = 0; i < editor.selected_gameObjects.size();++i) {
				string outputString = stringBuf;
				if (i > 0) {
					outputString.append(" (");
					outputString.append(std::to_string(i));
					outputString.append(")");
				}
				editor.command_controller.ExecuteCommand(COMMAND(CMD_ModifyInput<string>, GenericHandle{ editor.selected_gameObjects[i]->GetComponent<Name>() }, &editor.selected_gameObjects[i]->GetComponent<Name>()->name, outputString));

			}

		}


		if (ImGui::IsItemClicked()) {
			stringBuf = c_name->name;
		}
		else if (!ImGui::IsItemActive()) { //Disable assignment when editing text
			stringBuf = c_name->name;
		}



		
		string idName = std::to_string(gameObject.id);
		if (editor.selected_gameObjects.size() == 1)
			ImGui::Text("ID: %s", idName.data());
		else
			ImGui::TextDisabled("Multiple gameobjects selected");
	}

    void IGE_InspectorWindow::DisplayPrefabInstanceControls(Handle<PrefabInstance> c_prefab)
    {
        ImGui::Text("Prefab: ");
        ImGui::SameLine();
        ImGui::Text(Core::GetResourceManager().GetPath(c_prefab->prefab)->data());
    }

	void IGE_InspectorWindow::DisplayTransformComponent(Handle<Transform>& c_transform)
	{

		ImVec2 cursorPos = ImGui::GetCursorPos();
		ImVec2 cursorPos2{};
		IDE& editor = Core::GetSystem<IDE>();



		ImGui::SetCursorPosX(window_size.x - 20);
		if (ImGui::ArrowButton("AdditionalOptions", ImGuiDir_Down)) { //This is hidden, so lets redraw this as text after the collapsing header.

			ImGui::OpenPopup("AdditionalOptions");

		}

		ImGui::SetCursorPos(cursorPos);


		if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
		{
			vector<mat4>& originalMatrix = editor.selected_matrix;
			//This is dumped if no items are changed.
			//if (!isBeingModified) {
			//	originalMatrix.clear();
			//	for (Handle<GameObject> i : editor.selected_gameObjects) {
			//		originalMatrix.push_back(i->GetComponent<Transform>()->GlobalMatrix());
			//	}
			//}


			//Position
			float heightPos = ImGui::GetCursorPosY();
			ImGui::SetCursorPosY(heightPos + heightOffset);
			ImGui::Text("Position");
			ImGui::SameLine();

			//XYZ
			vec3& vecPosRef = c_transform->position;
			ImGui::PushItemWidth(window_size.x * float3Size - itemSpacing);
			ImGui::SetCursorPosX(widthOffset);
			ImGui::Text("X");
			ImGui::SameLine();

			ImGui::SetCursorPosY(ImGui::GetCursorPosY() - heightOffset);
			ImGui::PushID(&vecPosRef.x);
			if (ImGui::DragFloat("##X", &vecPosRef.x)) {
				for (Handle<GameObject> i : editor.selected_gameObjects) {
					i->GetComponent<Transform>()->position.x = vecPosRef.x;
				}
			}
			TransformModifiedCheck();


			ImGui::PopID();
			ImGui::SameLine();

			ImGui::Text("Y");
			ImGui::SameLine();
			ImGui::PushID(&vecPosRef.y);
			if (ImGui::DragFloat("##Y", &vecPosRef.y)) {
				for (Handle<GameObject> i : editor.selected_gameObjects) {
					i->GetComponent<Transform>()->position.y = vecPosRef.y;
				}
			}
			TransformModifiedCheck();


			ImGui::PopID();
			ImGui::SameLine();

			ImGui::Text("Z");
			ImGui::SameLine();
			ImGui::PushID(&vecPosRef.z);
			if (ImGui::DragFloat("##Z", &vecPosRef.z)) {
				for (Handle<GameObject> i : editor.selected_gameObjects) {
					i->GetComponent<Transform>()->position.z = vecPosRef.z;
				}
			}
			TransformModifiedCheck();


			ImGui::PopID();

			ImGui::PopItemWidth();
			//END XYZ


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
            deg x_deg{ original.x };
            if (ImGui::DragFloat("##RotationX", x_deg.data(), 1.0f)) {
				for (Handle<GameObject> i : editor.selected_gameObjects) { //Get each object rotation in euler, replace that euler axis and dump it back to rotation
					euler_angles eachGORotation { i->GetComponent<Transform>()->rotation };
                    eachGORotation.x = x_deg;
					i->GetComponent<Transform>()->rotation = quat{ eachGORotation };
				}
			}
			TransformModifiedCheck();


			ImGui::SameLine();

			ImGui::Text("Y");
			ImGui::SameLine();
            deg y_deg{ original.y };
            if (ImGui::DragFloat("##RotationY", y_deg.data(), 1.0f)) {
				for (Handle<GameObject> i : editor.selected_gameObjects) { //Get each object rotation in euler, replace that euler axis and dump it back to rotation
					euler_angles eachGORotation{ i->GetComponent<Transform>()->rotation };
					eachGORotation.y = y_deg;
					i->GetComponent<Transform>()->rotation = quat{ eachGORotation };
				}
			}
			TransformModifiedCheck();


			ImGui::SameLine();

			ImGui::Text("Z");
			ImGui::SameLine();
            deg z_deg{ original.z };
			if (ImGui::DragFloat("##RotationZ", z_deg.data(), 1.0f)) {
				for (Handle<GameObject> i : editor.selected_gameObjects) { //Get each object rotation in euler, replace that euler axis and dump it back to rotation
					euler_angles eachGORotation{ i->GetComponent<Transform>()->rotation };
					eachGORotation.z = z_deg;
					i->GetComponent<Transform>()->rotation = quat{ eachGORotation };
				}
			}
			TransformModifiedCheck();


			ImGui::PopItemWidth();

			//Scale
			heightPos = ImGui::GetCursorPosY();
			ImGui::SetCursorPosY(heightPos + heightOffset);
			ImGui::Text("Scale");
			ImGui::SameLine();

			//XYZ
			vec3& vecScaRef = c_transform->scale;
			ImGui::PushItemWidth(window_size.x* float3Size - itemSpacing);
			ImGui::SetCursorPosX(widthOffset);
			ImGui::Text("X");
			ImGui::SameLine();

			ImGui::SetCursorPosY(ImGui::GetCursorPosY() - heightOffset);
			ImGui::PushID(&vecScaRef.x);
			if (ImGui::DragFloat("##X", &vecScaRef.x)) {
				for (Handle<GameObject> i : editor.selected_gameObjects) {
					i->GetComponent<Transform>()->scale.x = vecScaRef.x;
				}
			}
			TransformModifiedCheck();


			ImGui::PopID();
			ImGui::SameLine();

			ImGui::Text("Y");
			ImGui::SameLine();
			ImGui::PushID(&vecScaRef.y);
			if (ImGui::DragFloat("##Y", &vecScaRef.y)) {
				for (Handle<GameObject> i : editor.selected_gameObjects) {
					i->GetComponent<Transform>()->scale.y = vecScaRef.y;
				}
			}
			TransformModifiedCheck();


			ImGui::PopID();
			ImGui::SameLine();

			ImGui::Text("Z");
			ImGui::SameLine();
			ImGui::PushID(&vecScaRef.z);
			if (ImGui::DragFloat("##Z", &vecScaRef.z)) {
				for (Handle<GameObject> i : editor.selected_gameObjects) {
					i->GetComponent<Transform>()->scale.z = vecScaRef.z;
				}
			}
			TransformModifiedCheck();

			if (hasChanged) {
				for (int i = 0; i < editor.selected_gameObjects.size();++i) {
					mat4 modifiedMat = editor.selected_gameObjects[i]->GetComponent<Transform>()->GlobalMatrix();
					editor.command_controller.ExecuteCommand(COMMAND(CMD_TransformGameObject, editor.selected_gameObjects[i], originalMatrix[i], modifiedMat));
					//Refresh the new matrix values
					editor.RefreshSelectedMatrix();
				}
				hasChanged		= false;
				//isBeingModified = false;
			}

			ImGui::PopID();

			ImGui::PopItemWidth();
			//END XYZ


		}

		cursorPos2 = ImGui::GetCursorPos();
		ImGui::SetCursorPos(cursorPos);
		ImGui::SetCursorPosX(window_size.x - 20);
		ImGui::Text("...");

		ImGui::SetCursorPos(cursorPos2);


		if (ImGui::BeginPopup("AdditionalOptions", ImGuiWindowFlags_None)) {
			if (ImGui::MenuItem("Reset")) {
				for (Handle<GameObject> i : editor.selected_gameObjects) {
					i->GetComponent<Transform>()->position	= vec3{ };
					i->GetComponent<Transform>()->rotation	= quat{ };
					i->GetComponent<Transform>()->scale		= vec3{ };
				}
			}
			ImGui::Separator();
			ImGui::EndPopup();
		}


	}

	void IGE_InspectorWindow::DisplayAnimatorComponent(Handle<Animator>& c_anim)
	{
		ImVec2 cursorPos = ImGui::GetCursorPos();
		ImVec2 cursorPos2{};
		ImGui::SetCursorPosX(window_size.x - 20);
		if (ImGui::ArrowButton("AdditionalOptions", ImGuiDir_Down)) { //This is hidden, so lets redraw this as text after the collapsing header.

			ImGui::OpenPopup("AdditionalOptions");

		}

		ImGui::SetCursorPos(cursorPos);


		if (ImGui::CollapsingHeader("Animation Controller", ImGuiTreeNodeFlags_DefaultOpen))
		{
			//Draw All your custom variables here.

			//ImGui::TextColored(ImVec4{ 1,0,0,1 }, "@IZAH/MAL: \n\tHelp me shift to correct place when free :3");
			if (ImGui::Button("Play"))
			{
				c_anim->Play(0);
			}
			ImGui::SameLine();
			if (ImGui::Button("Stop"))
			{
				c_anim->Stop();
			}
			ImGui::SameLine();
			if (ImGui::Button("Pause"))
			{
				c_anim->Pause();
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
				GenericHandle i = (*c_anim).GetHandle();
				componentNameMarkedForDeletion = (*i).type.name();
			}
			ImGui::EndPopup();
		}
	}

	void IGE_InspectorWindow::DisplayOtherComponent(GenericHandle& component)
	{
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

	void IGE_InspectorWindow::TransformModifiedCheck()
	{
		// if (ImGui::IsItemEdited()) {
		// 	isBeingModified = true;
		// }
		if (ImGui::IsItemDeactivatedAfterEdit()) {
			hasChanged = true;

		}
		else if (ImGui::IsItemDeactivated()) {
			hasChanged = false;
			//isBeingModified = false;

		}
	}


}
