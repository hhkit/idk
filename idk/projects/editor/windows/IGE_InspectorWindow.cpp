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
#include "anim/AnimationSystem.h"
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
            //Just show all components, Name and Transform first
            Handle<Name> c_name = gos[0]->GetComponent<Name>();
            if (c_name) {
                DisplayNameComponent(c_name);
            }

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
                if (component == c_name || component == c_transform || component.is_type<PrefabInstance>())
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
            Handle<Name> c_name = gos[0]->GetComponent<Name>();
            if (c_name) {
                DisplayNameComponent(c_name);

            }
            Handle<Transform> c_transform = gos[0]->GetComponent<Transform>();
            if (c_transform) {
                DisplayTransformComponent(c_transform);
            }


            //Just show similar components, based on first object
            span<GenericHandle> componentSpan = gos[0]->GetComponents();
            hash_set<string, std::hash<string>, std::equal_to<string>> similarComponentNames;
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
			ImGui::Text("ID: %s (scene: %d, index: %d, gen: %d)", 
				idName.data(), 
				s_cast<int>(gameObject.scene), 
				s_cast<int>(gameObject.index), 
				s_cast<int>(gameObject.gen));
		else
			ImGui::TextDisabled("Multiple gameobjects selected");
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



		ImGui::SetCursorPosX(window_size.x - 20);
		if (ImGui::ArrowButton("AdditionalOptions", ImGuiDir_Down)) { //This is hidden, so lets redraw this as text after the collapsing header.

			ImGui::OpenPopup("AdditionalOptions");

		}

		ImGui::SetCursorPos(cursorPos);

        ImGui::PushID("Transform");

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

			ImGui::DragFloat("Test Blend", &Core::GetSystem<AnimationSystem>()._blend, 0.01f, 0.0f, 1.0f);

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

        if (_prefab_inst)
            _prefab_curr_component = component;

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



    static string get_asset_name(const GenericResourceHandle& handle)
    {
        return std::visit([](auto h) -> string
        {
            auto name = h->Name();
            auto path = Core::GetResourceManager().GetPath(h);
            if (!name.empty())
                return string{ name };
            else if (path)
                return string{ PathHandle{ *path }.GetStem() };
            return "";
        }, handle);
    }

    void IGE_InspectorWindow::DisplayAsset(GenericResourceHandle handle)
    {
        ImGui::Text(get_asset_name(handle).c_str());

        std::visit([&](auto h)
        {
            using ResT = typename decltype(h)::Resource;
            auto path = Core::GetResourceManager().GetPath(h);

            ImGui::Text(format_name(reflect::get_type<ResT>().name()).c_str());
            ImGui::Text(string(handle.guid()).c_str());

            if constexpr (std::is_same_v<ResT, Prefab>)
                DisplayAsset(h);

            if constexpr (std::is_same_v<ResT, MaterialInstance>)
                DisplayAsset(h);

            if constexpr (std::is_same_v<ResT, Material>)
                DisplayAsset(h);

        }, handle);
    }

    void IGE_InspectorWindow::DisplayAsset(RscHandle<Prefab> prefab)
    {
        auto& data = prefab->data[0];

        auto name = reflect::get_type<Transform>().name();
        auto trans = data.FindComponent(name);
        if (trans.valid())
        {
            ImGui::PushID(0);
            if (ImGui::CollapsingHeader(name.data(), ImGuiTreeNodeFlags_DefaultOpen))
            {
                auto& c = trans.get<Transform>();

                const float item_width = ImGui::GetWindowContentRegionWidth() * 0.75f;
                const float pad_y = ImGui::GetStyle().FramePadding.y;

                ImGui::PushItemWidth(item_width);

                auto y = ImGui::GetCursorPosY();
                ImGui::SetCursorPosY(y + pad_y);
                ImGui::Text("Position");
                ImGui::SetCursorPosY(y);
                ImGui::SetCursorPosX(ImGui::GetWindowContentRegionWidth() - item_width);
                ImGuidk::DragVec3("##0", &c.position);

                y = ImGui::GetCursorPosY();
                ImGui::SetCursorPosY(y + pad_y);
                ImGui::Text("Rotation");
                ImGui::SetCursorPosY(y);
                ImGui::SetCursorPosX(ImGui::GetWindowContentRegionWidth() - item_width);
                ImGuidk::DragQuat("##1", &c.rotation);

                y = ImGui::GetCursorPosY();
                ImGui::SetCursorPosY(y + pad_y);
                ImGui::Text("Scale");
                ImGui::SetCursorPosY(y);
                ImGui::SetCursorPosX(ImGui::GetWindowContentRegionWidth() - item_width);
                if (ImGuidk::DragVec3("##2", &c.scale))
                {
                    PrefabUtility::PropagatePropertyToInstances(prefab, 0, name, "scale");
                    prefab->Dirty();
                }

                ImGui::PopItemWidth();
            }
            ImGui::PopID();
        }

        auto& components = prefab->data[0].components;

        int i = 0;
        for (auto& c : components)
        {
            ++i;
            if (c.type == reflect::get_type<Transform>() ||
                c.type == reflect::get_type<Name>())
                continue;

            auto cursor_pos = ImGui::GetCursorPos();

            ImGui::PushID(i);
            if (ImGui::CollapsingHeader(c.type.name().data(), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowItemOverlap))
            {
                if (displayVal(c))
                {
                    PrefabUtility::PropagatePrefabChangesToInstances(prefab);
                    prefab->Dirty();
                }
            }

            auto cursor_pos2 = ImGui::GetCursorPos();

            ImGui::SetCursorPosX(ImGui::GetWindowContentRegionWidth() - 20);
            ImGui::SetCursorPosY(cursor_pos.y);
            if (ImGui::Button("..."))
            {
                ImGui::OpenPopup("AdditionalOptions");
            }

            if (ImGui::BeginPopup("AdditionalOptions", ImGuiWindowFlags_None))
            {
                if (ImGui::MenuItem("Reset"))
                {

                }
                ImGui::Separator();
                if (ImGui::MenuItem("Remove Component"))
                {
                    PrefabUtility::RemoveComponentFromPrefab(prefab, 0, i - 1);
                    prefab->Dirty();
                }
                ImGui::EndPopup();
            }

            ImGui::SetCursorPos(cursor_pos2);

            ImGui::PopID();
        }

        ImGui::Separator();
        ImGui::SetCursorPosX(window_size.x * 0.25f);
        if (ImGui::Button("Add Component", ImVec2{ window_size.x * 0.5f, 0.0f }))
            ImGui::OpenPopup("AddComp");

        if (ImGui::BeginPopup("AddComp"))
        {
            for (const char* c_name : GameState::GetComponentNames())
            {
                string displayName = c_name;
                if (displayName == "Transform" ||
                    displayName == "Name" ||
                    displayName == "PrefabInstance")
                    continue;

                if (ImGui::MenuItem(displayName.c_str()))
                {
                    PrefabUtility::AddComponentToPrefab(prefab, 0, reflect::get_type(c_name).create());
                }
            }
            ImGui::EndPopup();
        }
    }

    void IGE_InspectorWindow::DisplayAsset(RscHandle<MaterialInstance> material)
    {
        const float item_width = ImGui::GetWindowContentRegionWidth() * item_width_ratio;
        const float pad_y = ImGui::GetStyle().FramePadding.y;

        auto graph = RscHandle<shadergraph::Graph>{ material->material };
        bool changed = false;

        for (auto& [name, u] : material->material->uniforms)
        {
            const auto y = ImGui::GetCursorPosY();

            ImGui::SetCursorPosY(y + pad_y);

            ImGui::PushID(name.c_str());

            bool has_override = material->uniforms.find(name) != material->uniforms.end();
            if (ImGui::Checkbox("##override", &has_override))
            {
                if (has_override)
                    material->uniforms[name] = material->material->uniforms[name].value;
                else
                    material->uniforms.erase(name);
            }
            ImGui::SameLine();

            ImGui::PushItemWidth(item_width);

            if (!has_override)
            {
                ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
                ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetColorU32(ImGuiCol_TextDisabled));
                ImGui::PushStyleColor(ImGuiCol_FrameBg, ImGui::GetColorU32(ImGuiCol_TitleBg));
            }

            ImGui::Text(name.c_str());
            ImGui::SetCursorPosY(y);
            ImGui::SetCursorPosX(ImGui::GetWindowContentRegionWidth() - item_width);

            switch (u.value.index())
            {
            case index_in_variant_v<float, UniformInstanceValue>:
            {
                auto val = std::get<index_in_variant_v<float, UniformInstanceValue>>(*material->GetUniform(name));
                if (ImGui::DragFloat("", &val, 0.01f))
                {
                    material->uniforms[name] = val;
                    changed = true;
                }
                break;
            }
            case index_in_variant_v<vec2, UniformInstanceValue>:
            {
                auto val = std::get<index_in_variant_v<vec2, UniformInstanceValue>>(*material->GetUniform(name));
                if (ImGuidk::DragVec2("", &val, 0.01f))
                {
                    material->uniforms[name] = val;
                    changed = true;
                }
                break;
            }
            case index_in_variant_v<vec3, UniformInstanceValue>:
            {
                auto val = std::get<index_in_variant_v<vec3, UniformInstanceValue>>(*material->GetUniform(name));
                if (ImGuidk::DragVec3("", &val, 0.01f))
                {
                    material->uniforms[name] = val;
                    changed = true;
                }
                break;
            }
            case index_in_variant_v<vec4, UniformInstanceValue>:
            {
                auto val = std::get<index_in_variant_v<vec4, UniformInstanceValue>>(*material->GetUniform(name));
                if (ImGui::DragFloat4("", val.data(), 0.01f))
                {
                    material->uniforms[name] = val;
                    changed = true;
                }
                break;
            }
            case index_in_variant_v<RscHandle<Texture>, UniformInstanceValue>:
            {
                auto val = std::get<index_in_variant_v<RscHandle<Texture>, UniformInstanceValue>>(*material->GetUniform(name));
                if (ImGuidk::InputResource("", &val))
                {
                    material->uniforms[name] = val;
                    changed = true;
                }
                break;
            }

            default:
                break;
            }

            if (!has_override)
            {
                ImGui::PopItemFlag();
                ImGui::PopStyleColor(2);
            }

            ImGui::PopItemWidth();
            ImGui::PopID();
        }

        if (changed)
            material->Dirty();
    }

    void IGE_InspectorWindow::DisplayAsset(RscHandle<Material> material)
    {
        const float item_width = ImGui::GetWindowContentRegionWidth() * item_width_ratio;
        const float pad_y = ImGui::GetStyle().FramePadding.y;

        auto graph = RscHandle<shadergraph::Graph>{ material };

        //for (auto& [name, u] : material->uniforms)
        //{
        //    if (u.index() == index_in_variant_v<RscHandle<Texture>, UniformInstance>)
        //        continue;

        //    auto y = ImGui::GetCursorPosY();

        //    ImGui::SetCursorPosY(y + pad_y);

        //    ImGui::PushID(name.c_str());
        //    ImGui::PushItemWidth(item_width);

        //    auto label = graph->parameters[std::stoi(name.data() + sizeof("_ub0._u") - 1)].name.c_str();
        //    switch (u.index())
        //    {
        //    case index_in_variant_v<float, UniformInstance>:
        //        ImGui::Text(label);
        //        ImGui::SetCursorPosY(y);
        //        ImGui::SetCursorPosX(ImGui::GetWindowContentRegionWidth() - item_width);
        //        ImGui::DragFloat("", &std::get<index_in_variant_v<float, UniformInstance>>(u), 0.01f);

        //    default:
        //        break;
        //    }

        //    ImGui::PopItemWidth();
        //    ImGui::PopID();
        //}
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
