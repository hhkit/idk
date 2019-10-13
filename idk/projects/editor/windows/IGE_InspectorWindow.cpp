//////////////////////////////////////////////////////////////////////////////////
//@file		IGE_InspectorWindow.cpp
//@author	Muhammad Izha B Rahim
//@param	Email : izha95\@hotmail.com
//@date		02 OCT 2019
//@brief	

/*
This window controls the top main menu bar. It also controls the docking
of the editor.
*/
//////////////////////////////////////////////////////////////////////////////////



#include "pch.h"
#include "IGE_InspectorWindow.h"

#include <IDE.h>
#include <gfx/ShaderGraph.h>
#include <editor/commands/CommandList.h>
#include <editor/imguidk.h>
#include <editor/windows/IGE_HierarchyWindow.h>
#include <editor/windows/IGE_ProjectWindow.h>
#include <editor/utils.h>
#include <common/TagSystem.h>
#include <anim/AnimationSystem.h>
#include <app/Application.h>
#include <ds/span.h>
#include <reflect/reflect.h>
#include <res/ResourceManager.h>
#include <scene/SceneManager.h>
#include <math/euler_angles.h>
#include <meta/variant.h>
#include <prefab/PrefabUtility.h>

#include <IncludeComponents.h>
#include <IncludeResources.h>

#include <imgui/imgui_stl.h>
#include <imgui/imgui_internal.h> //InputTextEx
#include <iostream>


namespace idk {

	IGE_InspectorWindow::IGE_InspectorWindow()
		:IGE_IWindow{ "Inspector##IGE_InspectorWindow",true,ImVec2{ 300,600 },ImVec2{ 450,150 } } 
	{	//Delegate Constructor to set window size
		// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.
		window_flags = ImGuiWindowFlags_NoCollapse;
	}

    void IGE_InspectorWindow::Initialize()
    {
        Core::GetSystem<IDE>().FindWindow<IGE_HierarchyWindow>()->OnGameObjectSelectionChanged.Listen([&]()
            {
                _displayed_asset = RscHandle<Texture>();
            });
        Core::GetSystem<IDE>().FindWindow<IGE_ProjectWindow>()->OnAssetsSelected.Listen([&](span<GenericResourceHandle> handles)
            {
                _displayed_asset = handles[0];
            });
    }

    void IGE_InspectorWindow::BeginWindow()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2{150.0f,100.0f});
	}



	void IGE_InspectorWindow::Update()
	{
		ImGui::PopStyleVar(1);

		IDE& editor = Core::GetSystem<IDE>();

        _prefab_inst = Handle<PrefabInstance>();
        if (_displayed_asset.guid())
        {
            DisplayAsset(_displayed_asset);
        }
        else
        {
            DisplayGameObjects(editor.selected_gameObjects);
        }
	}

    void IGE_InspectorWindow::DisplayGameObjects(vector<Handle<GameObject>> gos)
    {
        const size_t gameObjectsCount = gos.size();

        //DISPLAY
        if (gameObjectsCount == 1)
        {
            DisplayGameObjectHeader(gos[0]);

            if (const auto prefab_inst = gos[0]->GetComponent<PrefabInstance>())
            {
                _prefab_inst = prefab_inst;
                if (prefab_inst->object_index == 0)
                    DisplayPrefabInstanceControls(_prefab_inst);
            }

            Handle<Transform> c_transform = gos[0]->GetComponent<Transform>();
            if (c_transform) {
                DisplayTransformComponent(c_transform);
            }

            //Display remaining components here
            auto componentSpan = gos[0]->GetComponents();
            for (auto& component : componentSpan) {

                //Skip Name and Transform and PrefabInstance
                if (component == c_transform ||
                    component.is_type<PrefabInstance>() ||
                    component.is_type<Name>())
                    continue;

                if (component.is_type<Animator>())
                {
                    Handle<Animator> c_anim = gos[0]->GetComponent<Animator>();
                    DisplayAnimatorComponent(c_anim);
                    continue;
                }

                //COMPONENT DISPLAY 
                DisplayOtherComponent(component);
            }
        }
        else if (gameObjectsCount > 1)
        {
            //Just show all components, Name and Transform first
            //First gameobject takes priority. By default, name and transform will always be shown.
            DisplayGameObjectHeader(gos[0]);
            Handle<Transform> c_transform = gos[0]->GetComponent<Transform>();
            if (c_transform) {
                DisplayTransformComponent(c_transform);
            }


            //Just show similar components, based on first object
            span<GenericHandle> componentSpan = gos[0]->GetComponents();
            hash_set<string, std::hash<string>, std::equal_to<string>> similarComponentNames;
            for (GenericHandle component : componentSpan) {
                if (component.is_type<Name>())
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
                for (auto& gameObject : gos) {
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
                GenericHandle component = gos[0]->GetComponent(i);
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
            for (Handle<GameObject> i : gos)
                Core::GetSystem<IDE>().command_controller.ExecuteCommand(COMMAND(CMD_DeleteComponent, i, componentNameMarkedForDeletion));

            //Reset values
            componentNameMarkedForDeletion = {};
            isComponentMarkedForDeletion = false;

        }

        if (ImGui::BeginPopup("AddComp", ImGuiWindowFlags_None)) {
            span componentNames = GameState::GetComponentNames();
            for (const char* name : componentNames) {
                string displayName = name;
                if (displayName == "Transform" ||
                    displayName == "Name" ||
                    displayName == "Tag" ||
                    displayName == "Layer" ||
                    displayName == "PrefabInstance")
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
                    for (Handle<GameObject> i : gos)
                    {
                        Core::GetSystem<IDE>().command_controller.ExecuteCommand(COMMAND(CMD_AddComponent, i, string{ name }));
                    }
                }
            }
            ImGui::EndPopup();
        }
    }

    void IGE_InspectorWindow::DisplayGameObjectHeader(Handle<GameObject> game_object)
	{
		//The c_name is to just get the first gameobject
		static string stringBuf{};
		IDE& editor = Core::GetSystem<IDE>();
		//ImVec2 startScreenPos = ImGui::GetCursorScreenPos();

		ImGui::Text("Name");
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
			stringBuf = game_object->Name();
		}
		else if (!ImGui::IsItemActive()) { //Disable assignment when editing text
			stringBuf = game_object->Name();
		}

		string idName = std::to_string(game_object.id);
		if (editor.selected_gameObjects.size() == 1)
			ImGui::Text("ID: %s (scene: %d, index: %d, gen: %d)", 
				idName.data(), 
				s_cast<int>(game_object.scene),
				s_cast<int>(game_object.index),
				s_cast<int>(game_object.gen));
		else
			ImGui::TextDisabled("Multiple gameobjects selected");


        ImGui::Text("Tag");
        ImGui::SameLine();

        auto curr_tag = game_object->Tag();
        if (ImGui::BeginCombo("##tag", curr_tag.size() ? curr_tag.data() : "Untagged"))
        {
            if (ImGui::MenuItem("Untagged"))
                game_object->Tag("");
            for (const auto& tag : Core::GetSystem<TagSystem>().GetConfig().tags)
            {
                if (ImGui::MenuItem(tag.data()))
                    game_object->Tag(tag);
            }
            ImGui::EndCombo();
        }
	}

    void IGE_InspectorWindow::DisplayPrefabInstanceControls(Handle<PrefabInstance> c_prefab)
    {
        ImGui::Text("Prefab: ");
        ImGui::SameLine();
        auto path = *Core::GetResourceManager().GetPath(c_prefab->prefab);
        ImGui::Text(path.data() + path.rfind('/') + 1);

        ImGui::SameLine();
        if (ImGui::Button("Apply"))
        {
            PrefabUtility::ApplyPrefabInstance(c_prefab->GetGameObject());
        }
        ImGui::SameLine();
        if (ImGui::Button("Revert"))
        {
            PrefabUtility::RevertPrefabInstance(c_prefab->GetGameObject());
        }
    }

	void IGE_InspectorWindow::DisplayTransformComponent(Handle<Transform>& c_transform)
	{

		ImVec2 cursorPos = ImGui::GetCursorPos();
		ImVec2 cursorPos2{};
		IDE& editor = Core::GetSystem<IDE>();
        ImGui::PushID("Transform");

		if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowItemOverlap))
		{
			vector<mat4>& originalMatrix = editor.selected_matrix;
			//This is dumped if no items are changed.
			//if (!isBeingModified) {
			//	originalMatrix.clear();
			//	for (Handle<GameObject> i : editor.selected_gameObjects) {
			//		originalMatrix.push_back(i->GetComponent<Transform>()->GlobalMatrix());
			//	}
			//}


            auto& c = *c_transform;

            const float item_width = ImGui::GetWindowContentRegionWidth() * 0.75f;
            const float pad_y = ImGui::GetStyle().FramePadding.y;

            ImGui::PushItemWidth(item_width);

            auto y = ImGui::GetCursorPosY();
            ImGui::SetCursorPosY(y + pad_y);
            ImGui::Text("Position");
            ImGui::SetCursorPosY(y);
            ImGui::SetCursorPosX(ImGui::GetWindowContentRegionWidth() - item_width);
            auto origin = c.position;
            if (ImGuidk::DragVec3("##0", &c.position))
            {
                for (Handle<GameObject> i : editor.selected_gameObjects)
                {
                    i->GetComponent<Transform>()->position = c.position;
                }
            }
            TransformModifiedCheck();

            y = ImGui::GetCursorPosY();
            ImGui::SetCursorPosY(y + pad_y);
            ImGui::Text("Rotation");
            ImGui::SetCursorPosY(y);
            ImGui::SetCursorPosX(ImGui::GetWindowContentRegionWidth() - item_width);
            if (ImGuidk::DragQuat("##1", &c.rotation))
            {
                for (Handle<GameObject> i : editor.selected_gameObjects)
                {
                    i->GetComponent<Transform>()->rotation = c.rotation;
                }
            }
            TransformModifiedCheck();

            bool has_scale_override = false;
            if (_prefab_inst)
            {
                for (const auto& ov : _prefab_inst->overrides)
                {
                    if (ov.component_name == reflect::get_type<Transform>().name() &&
                        ov.property_path == "scale")
                    {
                        has_scale_override = true;
                        break;
                    }
                }
            }
            if (has_scale_override)
                ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetColorU32(ImGuiCol_PlotLinesHovered));

            y = ImGui::GetCursorPosY();
            ImGui::SetCursorPosY(y + pad_y);
            ImGui::Text("Scale");
            ImGui::SetCursorPosY(y);
            ImGui::SetCursorPosX(ImGui::GetWindowContentRegionWidth() - item_width);

            if (has_scale_override)
                ImGui::PopStyleColor();

            if (ImGuidk::DragVec3("##2", &c.scale))
            {
                for (Handle<GameObject> i : editor.selected_gameObjects)
                {
                    i->GetComponent<Transform>()->scale = c.scale;
                }
                if (_prefab_inst)
                {
                    PrefabUtility::RecordPrefabInstanceChange(c_transform->GetGameObject(), c_transform, "scale");
                }
            }
            TransformModifiedCheck();

            ImGui::PopItemWidth();



			if (hasChanged) {
				for (int i = 0; i < editor.selected_gameObjects.size();++i) {
					mat4 modifiedMat = editor.selected_gameObjects[i]->GetComponent<Transform>()->GlobalMatrix();
					editor.command_controller.ExecuteCommand(COMMAND(CMD_TransformGameObject, editor.selected_gameObjects[i], originalMatrix[i], modifiedMat));
					
				}
				//Refresh the new matrix values
				editor.RefreshSelectedMatrix();
				hasChanged		= false;
				//isBeingModified = false;
			}
		}

        ImGui::PopID();

		cursorPos2 = ImGui::GetCursorPos();
		ImGui::SetCursorPos(cursorPos);
		ImGui::SetCursorPosX(ImGui::GetWindowContentRegionWidth()-10);
		if (ImGui::Button("...")) {
			ImGui::OpenPopup("AdditionalOptions");

		}

		ImGui::SetCursorPos(cursorPos2);


		if (ImGui::BeginPopup("AdditionalOptions", ImGuiWindowFlags_None)) {
			if (ImGui::MenuItem("Reset")) {
				for (Handle<GameObject> i : editor.selected_gameObjects) {
					i->GetComponent<Transform>()->position	= vec3{ };
					i->GetComponent<Transform>()->rotation	= quat{ };
					i->GetComponent<Transform>()->scale		= vec3{ };
				}
			}
			MenuItem_CopyComponent(c_transform);
			MenuItem_PasteComponent();
			ImGui::Separator();
			ImGui::EndPopup();
		}


	}

	void IGE_InspectorWindow::DisplayAnimatorComponent(Handle<Animator>& c_anim)
	{
		ImVec2 cursorPos = ImGui::GetCursorPos();
		ImVec2 cursorPos2{};

		if (ImGui::CollapsingHeader("Animation Controller", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowItemOverlap))
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

			ImGui::DragFloat("Test Blend", &Core::GetSystem<AnimationSystem>()._blend, 0.01f, 0.0f, 1.0f);

		}

		cursorPos2 = ImGui::GetCursorPos();
		ImGui::SetCursorPos(cursorPos);
		ImGui::SetCursorPosX(ImGui::GetWindowContentRegionWidth() - 10);
		if (ImGui::Button("...")) {
			ImGui::OpenPopup("AdditionalOptions");

		}

		ImGui::SetCursorPos(cursorPos2);


		if (ImGui::BeginPopup("AdditionalOptions", ImGuiWindowFlags_None)) {
			if (ImGui::MenuItem("Reset")) {

			}
			ImGui::Separator();
			MenuItem_RemoveComponent(c_anim);
			MenuItem_CopyComponent(c_anim);
			MenuItem_PasteComponent();
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

        if (_prefab_inst)
            _prefab_curr_component = component;

		if (ImGui::CollapsingHeader(displayingComponent.c_str(), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowItemOverlap))
		{
			displayVal(*component);
		}

		cursorPos2 = ImGui::GetCursorPos();
		ImGui::SetCursorPos(cursorPos);
		ImGui::SetCursorPosX(ImGui::GetWindowContentRegionWidth() - 10);
		if (ImGui::Button("...")) {
			ImGui::OpenPopup("AdditionalOptions");

		}

		ImGui::SetCursorPos(cursorPos2);

		if (ImGui::BeginPopup("AdditionalOptions", ImGuiWindowFlags_None)) {
			if (ImGui::MenuItem("Reset")) {

			}
			ImGui::Separator();
			MenuItem_RemoveComponent(component);
			MenuItem_CopyComponent(component);
			MenuItem_PasteComponent();
			ImGui::EndPopup();
		}

		ImGui::PopID();
	}

	void IGE_InspectorWindow::MenuItem_RemoveComponent(GenericHandle i)
	{
		if (ImGui::MenuItem("Remove Component")) {
			isComponentMarkedForDeletion = true;
			componentNameMarkedForDeletion = (*i).type.name();
		}
	}

	void IGE_InspectorWindow::MenuItem_CopyComponent(GenericHandle i)
	{
		if (ImGui::MenuItem("Copy Component")) {
			Core::GetSystem<IDE>().copied_component.swap((*i).copy());
		}
	}

	void IGE_InspectorWindow::MenuItem_PasteComponent()
	{
		if (ImGui::MenuItem("Paste Component")) {
			//if (Core::GetSystem<IDE>().copied_component != reflect::dynamic{}) {
			//	reflect::dynamic newComponent{};
			//	parse_text(Core::GetSystem<IDE>().copied_component, newComponent);
			//	string compName = string{ newComponent.type.name() };
			//	std::cout << "Component pasted is " << compName << std::endl;
			//}
		}
	}



    bool IGE_InspectorWindow::displayVal(reflect::dynamic dyn)
    {
        const float item_width = ImGui::GetWindowContentRegionWidth() * item_width_ratio;
        const float pad_y = ImGui::GetStyle().FramePadding.y;

        bool outer_changed = false;

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
                _curr_property_stack.push_back(key);

                string curr_prop_path;
                for (const auto& prop : _curr_property_stack)
                {
                    curr_prop_path += prop;
                    curr_prop_path += '/';
                }
                curr_prop_path.pop_back();

                string keyName = format_name(key);

                if (keyName == "Guid")
                    return false;

                bool has_override = false;
                if (_prefab_inst)
                {
                    for (const auto& ov : _prefab_inst->overrides)
                    {
                        if (ov.component_name == (*_prefab_curr_component).type.name() &&
                            ov.property_path == curr_prop_path)
                        {
                            has_override = true;
                            break;
                        }
                    }
                }

                ImGui::BeginGroup();

                ImGui::SetCursorPosY(currentHeight + pad_y);
                if (has_override)
                    ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetColorU32(ImGuiCol_PlotLinesHovered));
                ImGui::Text(keyName.c_str());
                if (has_override)
                    ImGui::PopStyleColor();

                keyName.insert(0, "##"); //For Imgui stuff

                ImGui::SetCursorPosY(currentHeight);
                ImGui::SetCursorPosX(ImGui::GetWindowContentRegionWidth() - item_width);

                ImGui::PushID(keyName.c_str());
                ImGui::PushItemWidth(item_width);

                bool recurse = false;
                bool changed = false;
				[[maybe_unused]] bool changed_and_deactivated = false;

                //ALL THE TYPE STATEMENTS HERE
                if constexpr (std::is_same_v<T, float> || std::is_same_v<T, real>)
                {
                    changed |= ImGui::DragFloat(keyName.c_str(), &val);
                }
                else if constexpr (std::is_same_v<T, int>)
                {
                    changed |= ImGui::DragInt(keyName.c_str(), &val);
                }
                else if constexpr (std::is_same_v<T, bool>)
                {
                    changed |= ImGui::Checkbox(keyName.c_str(), &val);
                }
                else if constexpr (std::is_same_v<T, vec3>)
                {
                    changed |= ImGuidk::DragVec3(keyName.c_str(), &val);
                }
                else if constexpr (std::is_same_v<T, quat>)
                {
                    changed |= ImGuidk::DragQuat(keyName.c_str(), &val);
                }
                else if constexpr (std::is_same_v<T, color>)
                {
                    changed |= ImGui::ColorEdit4(keyName.c_str(), val.data());
                }
                else if constexpr (std::is_same_v<T, rad>)
                {
                    changed |= ImGui::SliderAngle(keyName.c_str(), val.data());
                }
                else if constexpr (is_template_v<T, RscHandle>)
                {
                    if (ImGuidk::InputResource(keyName.c_str(), &val))
                    {
                        changed = true;
                    }
                }
                else if constexpr (is_template_v<T, std::variant>)
                {
                    const int curr_ind = s_cast<int>(val.index());
                    int new_ind = curr_ind;

                    constexpr auto sz = reflect::detail::pack_size<T>::value; // THE FUUU?
                    using VarCombo = std::array<const char*, sz>;

                    static auto combo_items = []()-> VarCombo
                    {
                        constexpr auto sz = reflect::detail::pack_size<T>::value; // THE FUUU?
                        static std::array<string, sz> tmp_arr;
                        std::array<const char*, sz> retval{};

                        auto sp = reflect::unpack_types<T>();

                        for (size_t i = 0; i < sz; ++i)
                        {
                            tmp_arr[i] = format_name(sp[i].name());
                            retval[i] = tmp_arr[i].data();
                        }
                        return retval;
                    }();

                    if (ImGui::Combo(keyName.data(), &new_ind, combo_items.data(), static_cast<int>(sz)))
                    {
                        val = variant_construct<T>(new_ind);
                        changed = true;
                    }

                    recurse = true;
                }
                else if constexpr (is_sequential_container_v<T>)
                {
                    reflect::uni_container cont{ val };
                    ImGui::Button("+");
                    ImGui::Indent();
                    for (auto dyn : cont)
                    {
                        displayVal(dyn);
                    }
                    ImGui::Unindent();
                }
                else if constexpr (is_associative_container_v<T>)
                {
                    ImGui::Text("Associative Container");
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
                else
                {
                    ImGui::NewLine();
                    ImGui::Indent();
                    displayVal(val);
                    ImGui::Unindent();
                    /*ImGui::SetCursorPosY(currentHeight + heightOffset);
                    ImGui::TextDisabled("Member type not defined in IGE_InspectorWindow::Update");*/
                }

                ImGui::EndGroup();

                if (has_override && ImGui::BeginPopupContextItem("__context"))
                {
                    if (ImGui::MenuItem("Apply Property"))
                    {
                        PropertyOverride ov{ string((*_prefab_curr_component).type.name()), curr_prop_path };
                        PrefabUtility::ApplyPropertyOverride(_prefab_inst->GetGameObject(), ov);
                    }
                    if (ImGui::MenuItem("Revert Property"))
                    {
                        PropertyOverride ov{ string((*_prefab_curr_component).type.name()), curr_prop_path };
                        PrefabUtility::RevertPropertyOverride(_prefab_inst->GetGameObject(), ov);
                    }
                    ImGui::EndPopup();
                }

                outer_changed |= changed;
                if (changed && _prefab_inst)
                    PrefabUtility::RecordPrefabInstanceChange(_prefab_inst->GetGameObject(), _prefab_curr_component, curr_prop_path);

                _curr_property_stack.pop_back();
                ImGui::PopItemWidth();
                ImGui::PopID();

                return recurse;
            }

        });

        return outer_changed;
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
