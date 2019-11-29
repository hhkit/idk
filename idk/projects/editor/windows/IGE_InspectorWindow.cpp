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
#include <editor/windows/IGE_AnimatorWindow.h>
#include <editor/windows/IGE_ProjectSettings.h>
#include <editor/DragDropTypes.h>
#include <editor/utils.h>
#include <editor/ComponentIcons.h>

#include <common/TagManager.h>
#include <common/LayerManager.h>
#include <anim/AnimationSystem.h>
#include <app/Application.h>
#include <ds/span.h>
#include <reflect/reflect.h>
#include <res/ResourceManager.h>
#include <scene/SceneManager.h>
#include <math/euler_angles.h>
#include <meta/variant.h>
#include <script/MonoBehaviorEnvironment.h>
#include <prefab/PrefabUtility.h>

#include <IncludeComponents.h>
#include <IncludeResources.h>

#include <imgui/imgui_stl.h>
#include <imgui/imgui_internal.h> //InputTextEx
#include <iostream>

#include <gfx/GraphicsSystem.h>

namespace idk {

	IGE_InspectorWindow::IGE_InspectorWindow()
		:IGE_IWindow{ "Inspector##IGE_InspectorWindow",true,ImVec2{ 300,600 },ImVec2{ 450,150 } } 
	{	//Delegate Constructor to set window size
		// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.
        window_flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar;
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
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	}



	void IGE_InspectorWindow::Update()
	{
		ImGui::PopStyleVar(2);

        if (ImGui::BeginMenuBar())
        {
            ImGui::Checkbox("Debug", &_debug_mode);
            ImGui::EndMenuBar();
        }

        _prefab_inst = Handle<PrefabInstance>();
        if (_displayed_asset.guid())
        {
            const bool valid = std::visit([](auto h) { return bool(h); }, _displayed_asset);
            if (valid)
                DisplayAsset(_displayed_asset);
            else
                _displayed_asset = RscHandle<Texture>();
        }
        else
        {
            DisplayGameObjects(Core::GetSystem<IDE>().selected_gameObjects);
        }
	}

    void IGE_InspectorWindow::DisplayGameObjects(vector<Handle<GameObject>> gos)
    {
        const size_t gameObjectsCount = gos.size();

        if (gameObjectsCount == 0)
            return;

        // HEADER
        {
            ImGui::GetWindowDrawList()->ChannelsSplit(2);
            ImGui::GetWindowDrawList()->ChannelsSetCurrent(1);

            ImGui::BeginGroup();
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 8.0f);

            DisplayGameObjectHeader(gos[0]);

            if (gos[0].scene == Scene::prefab)
            {
                if (const auto prefab_inst = gos[0]->GetComponent<PrefabInstance>())
                {
                    if (prefab_inst->prefab)
                        _prefab_inst = gos[0]->GetComponent<PrefabInstance>();
                }
            }
            else if (const auto prefab_inst = gos[0]->GetComponent<PrefabInstance>())
            {
                if (prefab_inst->object_index == 0)
                    DisplayPrefabInstanceControls(prefab_inst);
				if (prefab_inst->prefab)
					_prefab_inst = prefab_inst;
            }

            ImGui::SetCursorPosY(ImGui::GetCursorPosY() - ImGui::GetStyle().ItemSpacing.y);
            ImGui::Dummy(ImVec2(ImGui::GetWindowContentRegionWidth(), 4.0f));
            ImGui::EndGroup();

            ImGui::GetWindowDrawList()->ChannelsSetCurrent(0);
            ImGui::GetWindowDrawList()->AddRectFilled(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), ImGui::GetColorU32(ImGuiCol_PopupBg));
            ImGui::GetWindowDrawList()->ChannelsMerge();

            ImGui::SetCursorPosY(ImGui::GetCursorPosY() - ImGui::GetStyle().ItemSpacing.y);
            ImGui::Separator();
        }

        // COMPONENTS

        ImGui::BeginChild("_inspector_inner");
        if (const auto rect_transform = gos[0]->GetComponent<RectTransform>())
            DisplayComponent(rect_transform);
        else
            DisplayComponent(gos[0]->GetComponent<Transform>());

        if (gameObjectsCount == 1)
        {
            //Display remaining components here
            auto componentSpan = gos[0]->GetComponents();
            for (auto& component : componentSpan) {

                //Skip Name and Transform and PrefabInstance
				if (component.is_type<Transform>() ||
					component.is_type<RectTransform>() ||
					component.is_type<PrefabInstance>() ||
					component.is_type<Name>() ||
					component.is_type<Tag>() ||
					component.is_type<Layer>()
                    )
					continue;

                //COMPONENT DISPLAY
                DisplayComponent(component);
            }

            if (_prefab_inst)
            {
                vector<int> removed, added;
                PrefabUtility::GetPrefabInstanceComponentDiff(gos[0], removed, added);
                if (removed.size())
                {
                    auto prefab_components = _prefab_inst->prefab->data[_prefab_inst->object_index].components;

                    ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetColorU32(ImGuiCol_TextDisabled));
                    ImGuidk::PushFont(FontType::Bold);
                    for (int i : removed)
                    {
                        ImGui::PushID(i);
                        ImGui::TreeNodeEx("", ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_Bullet |
                                          ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_AllowItemOverlap |
                                          ImGuiTreeNodeFlags_SpanFullWidth);

                        if (ImGui::BeginPopupContextItem())
                        {
                            if (ImGui::MenuItem("Revert Removed Component"))
                            {
                                gos[0]->AddComponent(prefab_components[i]);
                            }
                            if (ImGui::MenuItem("Apply Removed Component"))
                            {
                                PrefabUtility::RemoveComponentFromPrefab(_prefab_inst->prefab, _prefab_inst->object_index, i);
                            }
                            ImGui::EndPopup();
                        }

                        ImGui::SameLine(ImGui::GetStyle().IndentSpacing +
                            ImGui::GetStyle().FramePadding.y * 2 + ImGui::GetTextLineHeight() + 1);
                        ImGui::Text("%s (Removed)", prefab_components[i].type.name().data());
                        ImGui::PopID();
                    }
                    ImGui::PopFont();
                    ImGui::PopStyleColor();
                }
            }
        }
        else if (gameObjectsCount > 1)
        {
            //Just show similar components, based on first object
            span<GenericHandle> componentSpan = gos[0]->GetComponents();
            hash_set<string, std::hash<string>, std::equal_to<string>> similarComponentNames;
            for (GenericHandle component : componentSpan) {
				if (component.is_type<Name>() ||
                    component.is_type<Transform>() ||
                    component.is_type<Layer>() ||
                    component.is_type<Tag>() ||
                    component.is_type<PrefabInstance>()
                    )
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
                DisplayComponent(component);
                //ImGui::Text(i.c_str());
            }
        }

        //Add Component Button
        ImGui::SetCursorPosX(window_size.x * 0.25f);
        if (ImGui::Button("Add Component", ImVec2{ window_size.x * 0.5f,0.0f })) {
            ImGui::OpenPopup("AddComp");
        }

		ImGui::SetCursorPosX(window_size.x * 0.25f);
		if (ImGui::Button("Add Script", ImVec2{ window_size.x * 0.5f,0.0f })) {
			ImGui::OpenPopup("AddScript");
		}


        if (ImGui::BeginPopup("AddComp", ImGuiWindowFlags_None)) {
			ImGui::Text("Search bar:");
			ImGui::SameLine();
			bool value_changed = ImGui::InputTextEx("##component_textFilter", NULL, component_textFilter.InputBuf, IM_ARRAYSIZE(component_textFilter.InputBuf), ImVec2{100,0 }, ImGuiInputTextFlags_None);
			if (value_changed)
				component_textFilter.Build();


			
			string deco_text = "Component";
			if (component_textFilter.IsActive()) {
				deco_text = "Search Mode";
				ImGui::SameLine();
				if (ImGui::SmallButton("X##clear_component_textFilter"))
					component_textFilter.Clear();
			}

			ImGui::Separator();


			auto offset = ImGui::GetCursorPos();

			ImGui::SetCursorPos(ImVec2{ ImGui::GetWindowContentRegionWidth() * 0.5f - ImGui::CalcTextSize(deco_text.c_str()).x * 0.5f, offset.y + 5 });
			ImGui::Text(deco_text.c_str());
			ImGui::Separator();

            span componentNames = GameState::GetComponentNames();
            for (const char* name : componentNames) {
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


                if (ImGui::MenuItem(displayName.c_str())) {
                    //Add component
					int execute_counter = 0;
					CommandController& commandController = Core::GetSystem<IDE>().command_controller;
                    for (Handle<GameObject> i : gos)
                    {
                        commandController.ExecuteCommand(COMMAND(CMD_AddComponent, i, string{ name }));
						++execute_counter;
                    }

					commandController.ExecuteCommand(COMMAND(CMD_CallCommandAgain, execute_counter));
                }
            }
            ImGui::EndPopup();
        }

		if (ImGui::BeginPopup("AddScript", ImGuiWindowFlags_None)) {



			ImGui::Text("Search bar:");
			ImGui::SameLine();
			bool value_changed = ImGui::InputTextEx("##script_textFilter", NULL, script_textFilter.InputBuf, IM_ARRAYSIZE(script_textFilter.InputBuf), ImVec2{ 100,0 }, ImGuiInputTextFlags_None);
			if (value_changed)
				script_textFilter.Build();



			string deco_text = "Scripts";
			if (script_textFilter.IsActive()) {
				deco_text = "Search Mode";
				ImGui::SameLine();
				if (ImGui::SmallButton("X##clear_script_textFilter"))
					script_textFilter.Clear();
			}

			ImGui::Separator();


			auto offset = ImGui::GetCursorPos();

			ImGui::SetCursorPos(ImVec2{ ImGui::GetWindowContentRegionWidth() * 0.5f - ImGui::CalcTextSize(deco_text.c_str()).x * 0.5f, offset.y + 5 });
			ImGui::Text(deco_text.c_str());
			ImGui::Separator();

			auto* script_env = &Core::GetSystem<mono::ScriptSystem>().ScriptEnvironment();
			if (script_env == nullptr)
				ImGui::Text("Scripts not loaded!");

			span componentNames = script_env->GetBehaviorList();
			int execute_counter = 0;
			for (const char* name : componentNames) {

				if (!component_textFilter.PassFilter(name)) //skip if filtered
					continue;

				if (ImGui::MenuItem(name)) {
					for (Handle<GameObject> i : gos) {
						Core::GetSystem<IDE>().command_controller.ExecuteCommand(COMMAND(CMD_AddBehavior, i, string{ name }));
						++execute_counter;
					}
				}
			}
			if (execute_counter > 0) {
				CommandController& commandController = Core::GetSystem<IDE>().command_controller;
				commandController.ExecuteCommand(COMMAND(CMD_CallCommandAgain, execute_counter));
			}
			ImGui::EndPopup();
		}

        ImGui::EndChild();
    }

	void IGE_InspectorWindow::DisplayGameObjectHeader(Handle<GameObject> game_object)
	{
        const float left_offset = 40.0f;

		//The c_name is to just get the first gameobject
		static string stringBuf{};
		IDE& editor = Core::GetSystem<IDE>();
		//ImVec2 startScreenPos = ImGui::GetCursorScreenPos();

        ImGui::SetCursorPosX(left_offset - ImGui::GetFrameHeight() - ImGui::GetStyle().FramePadding.y * 2);
        bool is_active = game_object->ActiveSelf();
        if (ImGui::Checkbox("##Active", &is_active))
            game_object->SetActive(is_active);
		ImGui::SameLine();
        ImGui::PushItemWidth(-8.0f);
        if (game_object.scene == Scene::prefab)
            ImGuidk::PushDisabled();
		if (ImGui::InputText("##Name", &stringBuf, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_NoUndoRedo)) {
			//c_name->name = stringBuf;
			int execute_counter = 0;
			for (size_t i = 0; i < editor.selected_gameObjects.size();++i) {
				string outputString = stringBuf;
				if (i > 0) {
					outputString.append(" (");
					outputString.append(std::to_string(i));
					outputString.append(")");
				}
				editor.command_controller.ExecuteCommand(
                    COMMAND(CMD_ModifyProperty,
                            GenericHandle{ editor.selected_gameObjects[i]->GetComponent<Name>() },
                            "name",
                            string{ editor.selected_gameObjects[i]->GetComponent<Name>()->name },
                            string{ outputString })
                );
                editor.selected_gameObjects[i]->GetComponent<Name>()->name = outputString;
				++execute_counter;
			}
			CommandController& commandController = Core::GetSystem<IDE>().command_controller;
			commandController.ExecuteCommand(COMMAND(CMD_CallCommandAgain, execute_counter));
		}
        if (game_object.scene == Scene::prefab)
            ImGuidk::PopDisabled();
        ImGui::PopItemWidth();


		if (ImGui::IsItemClicked()) {
			stringBuf = game_object->Name();
		}
		else if (!ImGui::IsItemActive()) { //Disable assignment when editing text
			stringBuf = game_object->Name();
		}

        if (game_object.scene != Scene::prefab)
        {
            ImGui::SetCursorPosX(left_offset - ImGui::CalcTextSize("ID").x);
            ImGuidk::PushDisabled();
            ImGui::Text("ID");
            ImGui::SameLine();
            string idName = std::to_string(game_object.id);
            if (editor.selected_gameObjects.size() == 1)
                ImGui::Text("%s (scene: %d, index: %d, gen: %d)",
                    idName.data(),
                    s_cast<int>(game_object.scene),
                    s_cast<int>(game_object.index),
                    s_cast<int>(game_object.gen));
            else
                ImGui::TextDisabled("Multiple gameobjects selected");
            ImGuidk::PopDisabled();
        }


        ImGui::SetCursorPosX(left_offset - ImGui::CalcTextSize("Tag").x);
		ImGui::Text("Tag");
		ImGui::SameLine();

        ImGui::PushItemWidth(ImGui::GetWindowContentRegionWidth() * 0.5f - ImGui::GetCursorPosX());
        const auto curr_tag = game_object->Tag();
        if (ImGui::BeginCombo("##tag", curr_tag.size() ? curr_tag.data() : "Untagged"))
        {
            if (ImGui::MenuItem("Untagged"))
                game_object->Tag("");
            for (const auto& tag : Core::GetSystem<TagManager>().GetConfig().tags)
            {
                if (ImGui::MenuItem(tag.data()))
                    game_object->Tag(tag);
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Add Tag##_add_tag_"))
            {
                Core::GetSystem<IDE>().FindWindow<IGE_ProjectSettings>()->FocusConfig<TagManager>();
            }
            ImGui::EndCombo();
        }
        ImGui::PopItemWidth();
        ImGui::SameLine();

        ImGui::Text("Layer");
        ImGui::SameLine();

        ImGui::PushItemWidth(-8.0f);
        const auto curr_layer = game_object->Layer();
        const auto layer_name = Core::GetSystem<LayerManager>().LayerIndexToName(curr_layer);
        if (ImGui::BeginCombo("##layer", layer_name.data()))
        {
            const auto& layers = Core::GetSystem<LayerManager>().GetConfig().layers;
            for (LayerManager::layer_t i = 0; i < LayerManager::num_layers; ++i)
            {
                if (layers[i].empty())
                    continue;
                string label = serialize_text(i);
                label += ": ";
                label += layers[i];
                if (ImGui::MenuItem(label.c_str()))
                    game_object->Layer(i);
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Add Layer##_add_layer_"))
            {
                Core::GetSystem<IDE>().FindWindow<IGE_ProjectSettings>()->FocusConfig<TagManager>();
            }
            ImGui::EndCombo();
        }
        ImGui::PopItemWidth();
	}

    void IGE_InspectorWindow::DisplayPrefabInstanceControls(Handle<PrefabInstance> c_prefab)
    {
        const float left_offset = 40.0f;
        ImGui::SetCursorPosX(left_offset - ImGui::CalcTextSize("Prefab").x);

        if (!c_prefab->prefab)
        {
            ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetColorU32(ImGuiCol_TextDisabled));
            ImGui::Text("Missing");
            ImGui::PopStyleColor();
            return;
        }

        _prefab_inst = c_prefab;
        ImGui::Text("Prefab");
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

    template<>
	void IGE_InspectorWindow::DisplayComponentInner(Handle<Transform> c_transform)
	{

		ImVec2 cursorPos = ImGui::GetCursorPos();
		ImVec2 cursorPos2{};
		IDE& editor = Core::GetSystem<IDE>();

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

        ImGui::PushItemWidth(-4.0f);

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
			int execute_counter = 0;
			for (int i = 0; i < editor.selected_gameObjects.size();++i) {
				mat4 modifiedMat = editor.selected_gameObjects[i]->GetComponent<Transform>()->GlobalMatrix();
				editor.command_controller.ExecuteCommand(COMMAND(CMD_TransformGameObject, editor.selected_gameObjects[i], originalMatrix[i], modifiedMat));
				++execute_counter;
			}
			CommandController& commandController = Core::GetSystem<IDE>().command_controller;
			commandController.ExecuteCommand(COMMAND(CMD_CallCommandAgain, execute_counter));
			//Refresh the new matrix values
			editor.RefreshSelectedMatrix();
			hasChanged		= false;
			//isBeingModified = false;
		}
	}


    template<>
    void IGE_InspectorWindow::DisplayComponentInner(Handle<RectTransform> c_rt)
    {
        if (c_rt->GetGameObject()->HasComponent<Canvas>())
        {
            ImGuidk::PushDisabled();
            ImGui::Text("Values driven by Canvas.");
            ImGuidk::PopDisabled();
            return;
        }

        const float region_width = ImGui::GetWindowContentRegionWidth();
        
        const auto w = region_width * 0.3f;
        ImGui::PushItemWidth(w);

        bool has_override = _prefab_inst &&
            (_prefab_inst->HasOverride((*_curr_component).type.name(), "offset_min", 0) ||
             _prefab_inst->HasOverride((*_curr_component).type.name(), "offset_max", 0));

        bool changed = false;

        ImGui::BeginGroup();

        if (c_rt->anchor_min.y != c_rt->anchor_max.y)
        {
            ImGui::SetCursorPosX(region_width * 0.5f - ImGui::CalcTextSize("T").x * 0.5f);
            ImGui::Text("T");
            ImGui::SetCursorPosX(region_width * 0.35f);
            changed |= ImGui::DragFloat("##top", &c_rt->offset_max.y);
        }
        else
        {
            ImGui::SetCursorPosX(region_width * 0.5f - ImGui::CalcTextSize("Y").x * 0.5f);
            ImGui::Text("Y");
            ImGui::SetCursorPosX(region_width * 0.35f);
            const float pivot_y = c_rt->_local_rect.position.y + c_rt->pivot.y * c_rt->_local_rect.size.y;
            float pos_y = pivot_y;
            if (ImGui::DragFloat("##pos_y", &pos_y))
            {
                float dy = pos_y - pivot_y;
                c_rt->offset_min.y += dy;
                c_rt->offset_max.y += dy;
                changed = true;
            }
        }

        if (c_rt->anchor_min.x != c_rt->anchor_max.x)
        {
            ImGui::SetCursorPosX(region_width * 0.35f - w * 0.75f - ImGui::CalcTextSize("L").x - ImGui::GetStyle().ItemSpacing.x);
            ImGui::Text("L");
            ImGui::SameLine();
            changed |= ImGui::DragFloat("##left", &c_rt->offset_min.x);
            ImGui::SameLine();
            ImGui::SetCursorPosX(region_width * 0.35f + w * 0.75f);
            changed |= ImGui::DragFloat("##right", &c_rt->offset_max.x);
            ImGui::SameLine();
            ImGui::Text("R");
        }
        else
        {
            ImGui::SetCursorPosX(region_width * 0.35f - w * 0.75f - ImGui::CalcTextSize("X").x - ImGui::GetStyle().ItemSpacing.x);
            ImGui::Text("X");
            ImGui::SameLine();
            
            const float pivot_x = c_rt->_local_rect.position.x + c_rt->pivot.x * c_rt->_local_rect.size.x;
            float pos_x = pivot_x;
            if (ImGui::DragFloat("##pos_x", &pos_x))
            {
                float dx = pos_x - pivot_x;
                c_rt->offset_min.x += dx;
                c_rt->offset_max.x += dx;
                changed = true;
            }
            ImGui::SameLine();

            float width = c_rt->_local_rect.size.x;
            ImGui::SetCursorPosX(region_width * 0.35f + w * 0.75f);
            if (ImGui::DragFloat("##width", &width))
            {
                c_rt->offset_min.x = pos_x - c_rt->pivot.x * width;
                c_rt->offset_max.x = pos_x + (1.0f - c_rt->pivot.x) * width;
                changed = true;
            }
            ImGui::SameLine();
            ImGui::Text("W");
        }

        if (c_rt->anchor_min.y != c_rt->anchor_max.y)
        {
            ImGui::SetCursorPosX(region_width * 0.35f);
            changed |= ImGui::DragFloat("##bot", &c_rt->offset_min.y);
            ImGui::SetCursorPosX(region_width * 0.5f - ImGui::CalcTextSize("B").x * 0.5f);
            ImGui::Text("B");
        }
        else
        {
            float height = c_rt->_local_rect.size.y;
            ImGui::SetCursorPosX(region_width * 0.35f);
            if (ImGui::DragFloat("##height", &height))
            {
                const float pivot_y = c_rt->_local_rect.position.y + c_rt->pivot.y * c_rt->_local_rect.size.y;
                c_rt->offset_min.y = pivot_y - c_rt->pivot.y * height;
                c_rt->offset_max.y = pivot_y + (1.0f - c_rt->pivot.y) * height;
                changed = true;
            }
            ImGui::SetCursorPosX(region_width * 0.5f - ImGui::CalcTextSize("B").x * 0.5f);
            ImGui::Text("H");
        }

        ImGui::PopItemWidth();

        ImGui::EndGroup();

        if (has_override && ImGui::BeginPopupContextItem("__context"))
        {
            if (ImGui::MenuItem("Apply Property"))
            {
                PropertyOverride ov{ string((*_curr_component).type.name()), "offset_min", 0 };
                PrefabUtility::ApplyPropertyOverride(_prefab_inst->GetGameObject(), ov);
                ov.property_path = "offset_max";
                PrefabUtility::ApplyPropertyOverride(_prefab_inst->GetGameObject(), ov);
            }
            if (ImGui::MenuItem("Revert Property"))
            {
                PropertyOverride ov{ string((*_curr_component).type.name()), "offset_min", 0 };
                PrefabUtility::RevertPropertyOverride(_prefab_inst->GetGameObject(), ov);
                ov.property_path = "offset_max";
                PrefabUtility::RevertPropertyOverride(_prefab_inst->GetGameObject(), ov);
            }
            ImGui::EndPopup();
        }

        if (changed && _prefab_inst)
        {
            PrefabUtility::RecordPrefabInstanceChange(_prefab_inst->GetGameObject(), _curr_component, "offset_min");
            PrefabUtility::RecordPrefabInstanceChange(_prefab_inst->GetGameObject(), _curr_component, "offset_max");
        }

        if (has_override)
        {
            ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(ImGui::GetWindowPos().x, ImGui::GetItemRectMin().y) + ImVec2(4.0f, 0),
                ImVec2(ImGui::GetWindowPos().x, ImGui::GetItemRectMin().y) + ImVec2(4.0f, 0) + ImVec2(4.0f, ImGui::GetItemRectSize().y),
                ImGui::GetColorU32(ImGuiCol_PlotLinesHovered));
        }



        DisplayStack display(*this);

        _curr_property_stack.push_back("anchor_min"); display.GroupBegin(); display.Label("Anchor Min"); display.ItemBegin(true);
        changed = ImGuidk::DragVec2("##anchor_min", &c_rt->anchor_min);
        display.ItemEnd(); display.GroupEnd(changed); _curr_property_stack.pop_back();

        _curr_property_stack.push_back("anchor_max"); display.GroupBegin(); display.Label("Anchor Max"); display.ItemBegin(true);
        changed = ImGuidk::DragVec2("##anchor_max", &c_rt->anchor_max);
        display.ItemEnd(); display.GroupEnd(changed); _curr_property_stack.pop_back();

        _curr_property_stack.push_back("pivot"); display.GroupBegin(); display.Label("Pivot"); display.ItemBegin(true);
        changed = ImGuidk::DragVec2("##pivot", &c_rt->pivot, 0.01f, 0, 1.0f);
        display.ItemEnd(); display.GroupEnd(changed); _curr_property_stack.pop_back();

        

        // z, scale, rot
        IDE& editor = Core::GetSystem<IDE>();
        auto& c = *c_rt->GetGameObject()->Transform();

        const float item_width = ImGui::GetWindowContentRegionWidth() * 0.75f;
        const float pad_y = ImGui::GetStyle().FramePadding.y;

        ImGui::PushItemWidth(-4.0f);

        _curr_component = c.GetHandle();
        _prefab_curr_component_nth = 0;
        _curr_property_stack.push_back("position"); _curr_property_stack.push_back("z");
        display.GroupBegin(); display.Label("Pos Z"); display.ItemBegin(true);
        changed = ImGui::DragFloat("##pos_z", &c.position.z);
        display.ItemEnd(); display.GroupEnd(changed); _curr_property_stack.pop_back(); _curr_property_stack.pop_back();

        changed = false;
        _curr_property_stack.push_back("rotation");
        display.GroupBegin(); display.Label("Rotation"); display.ItemBegin(true);
        if (ImGuidk::DragQuat("##rot", &c.rotation))
        {
            changed = true;
            for (Handle<GameObject> i : editor.selected_gameObjects)
                i->GetComponent<Transform>()->rotation = c.rotation;
        }
        display.ItemEnd(); display.GroupEnd(changed); _curr_property_stack.pop_back();

        changed = false;
        _curr_property_stack.push_back("scale");
        display.GroupBegin(); display.Label("Scale"); display.ItemBegin(true);
        if (ImGuidk::DragVec3("##scl", &c.scale))
        {
            changed = true;
            for (Handle<GameObject> i : editor.selected_gameObjects)
                i->GetComponent<Transform>()->rotation = c.rotation;
        }
        display.ItemEnd(); display.GroupEnd(changed); _curr_property_stack.pop_back();
    }


    template<>
	void IGE_InspectorWindow::DisplayComponentInner(Handle<Animator> c_anim)
	{
		ImVec2 cursorPos = ImGui::GetCursorPos();
		ImVec2 cursorPos2{};
		DisplayStack display{*this};


		_curr_property_stack.emplace_back("layers");
		display.GroupBegin();
		auto title_pos = ImGui::GetCursorPos();
		bool tree_open = ImGui::CollapsingHeader("##animation_layers", ImGuiTreeNodeFlags_AllowItemOverlap);
		// ImGui::SetCursorPos(title_pos);
		ImGui::SameLine();
		display.Label("Layers");
		ImGui::NewLine();
		if (tree_open)
		{
			ImGui::Indent();
			for (auto& layer : c_anim->layers)
			{
				// ImGui::Separator(false);
				ImGui::PushID(&layer);
				
				ImGui::Text(layer.name.data());
				ImGui::SameLine();
				const auto arrow_pos = ImVec2{ ImGui::GetCurrentWindowRead()->DC.CursorPos.x,
									   ImGui::GetCurrentWindowRead()->DC.CursorPos.y + ImGui::GetFontSize() * 0.25f };
				ImGui::RenderArrow(arrow_pos, ImGuiDir_Right, 0.7f);
				ImGui::SameLine(0.0f, ImGui::GetFrameHeight() + ImGui::GetStyle().FramePadding.x * 2);
				ImGui::Text(c_anim->CurrentStateName().data());

				ImGui::ProgressBar(layer.curr_state.normalized_time, ImVec2{ -1, 3 }, nullptr);
				ImGui::PopID();
				ImGui::Separator(false);
			}
			ImGui::Unindent();
		}
		display.GroupEnd(false);
		_curr_property_stack.pop_back();

		static char buf[50];
		const auto display_param = [&](auto& param) -> bool
		{
			using T = std::decay_t<decltype(param)>;
			bool ret_val = false;

			ImGui::PushID(&param);
			strcpy_s(buf, param.name.data());
			ImGui::BeginGroup();
			const float align_widget = ImGui::GetContentRegionAvailWidth() * 0.7f;
			const float widget_size = ImGui::GetContentRegionAvailWidth() * 0.3f;

			ImGui::PushItemWidth(ImGui::GetContentRegionAvailWidth() * 0.5f);
			if (ImGui::InputText("##rename_param", buf, 50, ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_NoUndoRedo | ImGuiInputTextFlags_EnterReturnsTrue))
			{
				ret_val |= c_anim->RenameParam<T>(param.name, buf);
			}

			ImGui::PopItemWidth();
			ImGui::SameLine(align_widget);

			auto& val = c_anim->IsPlaying() ? param.val : param.def_val;
			if constexpr (std::is_same_v <T, anim::IntParam>)
			{
				ImGui::PushItemWidth(widget_size);
				ret_val |= ImGui::DragInt("##param_input", &val) && !c_anim->IsPlaying();
				ImGui::PopItemWidth();
			}
			else if constexpr (std::is_same_v < T, anim::FloatParam>)
			{
				ImGui::PushItemWidth(widget_size);
				ret_val |= ImGui::DragFloat("##param_input", &val, 0.01f) && !c_anim->IsPlaying();
				ImGui::PopItemWidth();
			}
			else if constexpr (std::is_same_v <T, anim::BoolParam>)
			{
				ret_val |= ImGui::Checkbox("##param_input", &val) && !c_anim->IsPlaying();
			}
			else if constexpr (std::is_same_v <T, anim::TriggerParam>)
			{
				if (ImGui::RadioButton("##param_input", val))
				{
					ret_val = true && !c_anim->IsPlaying();
					val = !val;
				}
			}
			else
				throw("???");

			ImGui::Separator(false);

			ImGui::EndGroup();
			ImGui::Unindent();
			ImGui::PopID();

			return ret_val;
		};

		_curr_property_stack.emplace_back("parameters");
		bool params_changed = false;
		display.GroupBegin();
		title_pos = ImGui::GetCursorPos();
		tree_open = ImGui::CollapsingHeader("##animation_params", ImGuiTreeNodeFlags_AllowItemOverlap);
		// ImGui::SetCursorPos(title_pos);
		ImGui::SameLine();
		display.Label("Parameters");
		ImGui::NewLine();

		if (tree_open)
		{
			if (ImGui::CollapsingHeader("Int"))
			{
				for (auto& param : c_anim->GetParamTable<anim::IntParam>())
				{
					params_changed |= display_param(param.second);
				}
			}

			if (ImGui::CollapsingHeader("Float"))
			{
				for (auto& param : c_anim->GetParamTable<anim::FloatParam>())
				{
					params_changed |= display_param(param.second);
				}
			}

			if (ImGui::CollapsingHeader("Bool"))
			{
				for (auto& param : c_anim->GetParamTable<anim::BoolParam>())
				{
					params_changed |= display_param(param.second);
				}
			}

			if (ImGui::CollapsingHeader("Trigger"))
			{
				for (auto& param : c_anim->GetParamTable<anim::TriggerParam>())
				{
					params_changed |= display_param(param.second);
				}
			}
		}
		ImGui::Separator(false);
		display.GroupEnd(params_changed);
		_curr_property_stack.pop_back();

		// layers override check
		if (ImGui::Button("Open Animator Window"))
		{
			Core::GetSystem<IDE>().FindWindow<IGE_AnimatorWindow>()->is_open = true;
		}
		//ImGui::SameLine(0.0f, 5.0f);
		
		ImGui::Text("Preview"); ImGui::SameLine();
		if (ImGui::Checkbox("##preview", &c_anim->preview_playback)) 
		{
			c_anim->OnPreview();
		}
		ImGui::NewLine();
	}

    template<>
	void IGE_InspectorWindow::DisplayComponentInner(Handle<Bone> c_bone)
	{
		//Draw All your custom variables here.
		ImGui::Text("Bone Name: ");
		ImGui::SameLine();
		ImGui::Text(c_bone->bone_name.c_str());
		ImGui::Text("Bone Index: %d", c_bone->bone_index);
	}

	template<>
	void IGE_InspectorWindow::DisplayComponentInner(Handle<TextMesh> c_font)
	{
        constexpr CustomDrawFn draw_text = [](const reflect::dynamic& val)
        {
            return ImGui::InputTextMultiline("", &val.get<string>());
        };
        InjectDrawTable table{ { "text", draw_text } };
        DisplayVal(*c_font, &table);
	}

    template<>
    void IGE_InspectorWindow::DisplayComponentInner(Handle<Text> c_text)
    {
        constexpr CustomDrawFn draw_text = [](const reflect::dynamic& val)
        {
            return ImGui::InputTextMultiline("", &val.get<string>());
        };
        constexpr CustomDrawFn draw_alignment = [](const reflect::dynamic& val)
        {
            auto& anchor = val.get<TextAnchor>();
            return ImGuidk::EnumCombo("", &anchor);
        };
        InjectDrawTable table{ { "text", draw_text }, { "alignment", draw_alignment } };
        DisplayVal(*c_text, &table);
    }

	template<>
	void IGE_InspectorWindow::DisplayComponentInner(Handle<AudioSource> c_audiosource)
	{
		static Handle<AudioSource> static_audiosource{};
		static_audiosource = c_audiosource;
		constexpr auto draw_audio_list = [](const reflect::dynamic& dyn) -> bool
		{
			bool changed = false;

			auto& audio_clip_list = dyn.get<vector<RscHandle<AudioClip>>>();
			if (ImGui::Button("Add AudioClip")) {
				audio_clip_list.emplace_back(RscHandle<AudioClip>());
				changed = true;
			}

			ImGui::Separator();
			if (!audio_clip_list.empty()) {
				ImGui::Text("AudioClips");
				ImGui::BeginChild("AudioClips", ImVec2(ImGui::GetWindowContentRegionWidth() - 10, 200), true);

				for (auto i = 0; i < audio_clip_list.size(); ++i) {
					string txt = "[" + std::to_string(i) + "]";
					if (ImGuidk::InputResource(txt.c_str(), &audio_clip_list[i])) {
						//Stop playing before switching sounds!
						changed = true;
					}
					ImGui::SameLine();
					ImGui::PushID(i);
					if (ImGui::SmallButton("X")) {
						static_audiosource->RemoveAudioClip(i);
						ImGui::PopID();
						changed = true;
						break;
					}

					ImGui::SameLine();
					if (static_audiosource->IsAudioClipPlaying(i)) {
						if (ImGui::SmallButton("||")) {
							static_audiosource->Stop(i);
						}
					}
					else {
						if (ImGui::ArrowButton("Play", ImGuiDir_Right)) {
							static_audiosource->Play(i);
						}
					}
					ImGui::PopID();

				}

				ImGui::EndChild();
			}

			return changed;
		};

		InjectDrawTable table{
			{ "audio_clip_list", draw_audio_list }
		};

		DisplayVal(*c_audiosource, &table);

	}

    template<>
    void IGE_InspectorWindow::DisplayComponentInner(Handle<ParticleSystem> c_ps)
    {
        if (c_ps->state == ParticleSystem::Playing && ImGui::Button("Pause"))
            c_ps->Pause();
        else if (c_ps->state != ParticleSystem::Playing && ImGui::Button("Play"))
            c_ps->Play();
        ImGui::SameLine();
        if (ImGui::Button("Restart"))
        {
            c_ps->Stop();
            c_ps->Play();
        }
        ImGui::SameLine();
        if (ImGui::Button("Stop"))
            c_ps->Stop();

        if (c_ps->state > ParticleSystem::Stopped)
        {
            ImGui::SameLine();
            ImGui::Text("%.2fs", c_ps->time);
        }

        ImGuidk::PushFont(FontType::Smaller);

        _curr_property_stack.push_back("main");
        DisplayVal(c_ps->main);
        _curr_property_stack.pop_back();

        const auto display = [&](const char* title, reflect::dynamic dyn, InjectDrawTable* inject = nullptr)
        {
            ImGui::PushID(title);

            const auto x = ImGui::GetCursorPosX();
            ImGui::SetCursorPosX(x - 1.0f);

            ImGui::GetCurrentWindow()->WorkRect.Max.x -= 5.0f; // hack
            ImGui::PushStyleColor(ImGuiCol_Header, ImGui::GetColorU32(ImGuiCol_TitleBgActive));
            const bool open = ImGui::CollapsingHeader(format_name(title).c_str(), ImGuiTreeNodeFlags_AllowItemOverlap);
            ImGui::PopStyleColor();
            ImGui::GetCurrentWindow()->WorkRect.Max.x += 5.0f; // hack

            ImGui::SameLine(x + ImGui::GetStyle().ItemInnerSpacing.x);
            ImGui::Checkbox("##enabled", &dyn.get_property("enabled").value.get<bool>());

            if (open)
            {
                _curr_property_stack.push_back(title);
                DisplayVal(std::move(dyn), inject);
                _curr_property_stack.pop_back();
            }

            ImGui::PopID();
        };

        static Handle<ParticleSystem> _static_ps_handle{};
        _static_ps_handle = c_ps;
        constexpr auto draw_bursts = [](const reflect::dynamic& val) -> bool
        {
            bool changed = false;
            auto& bursts = val.get<vector<EmissionModule::Burst>>();
            if (ImGui::Button("+"))
            {
                bursts.emplace_back();
                changed = true;
            }
            if (bursts.size())
            {
                ImGui::SameLine();
                if (ImGui::Button("-"))
                {
                    bursts.pop_back();
                    changed = true;
                }
            }

            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
            const auto w = ImGui::CalcItemWidth();
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + w * 0.1f - ImGui::CalcTextSize("Time").x * 0.5f);
            ImGui::Text("Time");
            ImGui::SameLine(w * 0.3f - ImGui::CalcTextSize("Count").x * 0.5f);
            ImGui::Text("Count");
            ImGui::SameLine(w * 0.5f - ImGui::CalcTextSize("Cycles").x * 0.5f);
            ImGui::Text("Cycles");
            ImGui::SameLine(w * 0.7f - ImGui::CalcTextSize("Interval").x * 0.5f);
            ImGui::Text("Interval");
            ImGui::SameLine(w * 0.9f - ImGui::CalcTextSize("Probability").x * 0.5f);
            ImGui::Text("Probability");

            ImGui::PushItemWidth(w * 0.2f);
            for(int i = 0; i < bursts.size(); ++i)
            {
                auto& burst = bursts[i];
                ImGui::PushID(i);
                changed |= ImGui::DragFloat("##time", &burst.time, 0.01f, 0, _static_ps_handle->main.duration);
                ImGui::SameLine(); changed |= ImGui::DragInt("##count", &burst.count, 1.0f, 1, _static_ps_handle->main.max_particles);
                ImGui::SameLine(); changed |= ImGui::DragInt("##cycles", &burst.cycles, 1.0f, 1, 32);
                ImGui::SameLine(); changed |= ImGui::DragFloat("##interval", &burst.interval, 0.01f, 0, _static_ps_handle->main.duration);
                ImGui::SameLine(); changed |= ImGui::DragFloat("##probability", &burst.probability, 0.01f, 0, 1.0f);
                ImGui::PopID();
            }
            ImGui::PopItemWidth();
            ImGui::PopStyleVar();

            return changed;
        };
        InjectDrawTable inject{ { "emission/bursts", CustomDrawFn(draw_bursts) } };

        display("emission", c_ps->emission, &inject);
        display("shape", c_ps->shape);
        display("velocity_over_lifetime", c_ps->velocity_over_lifetime);
        display("color_over_lifetime", c_ps->color_over_lifetime);
        display("size_over_lifetime", c_ps->size_over_lifetime);
        display("rotation_over_lifetime", c_ps->rotation_over_lifetime);
        display("renderer", c_ps->renderer);

        ImGui::PopFont();
    }

	void IGE_InspectorWindow::DisplayComponent(GenericHandle component)
	{
		//COMPONENT DISPLAY
        ImGui::PushID(component.type);
        ImGui::PushID(component.index);
		string displayingComponent = [&]() ->string
		{
			auto type = (*component).type;
			return type.is<mono::Behavior>() ? string{ handle_cast<mono::Behavior>(component)->TypeName() } + "(Script)" : string{ type.name() };
		}();
		const string fluffText{ "idk::" };
		std::size_t found = displayingComponent.find(fluffText);

		if (found != std::string::npos)
			displayingComponent.erase(found, fluffText.size());

		ImVec2 cursorPos = ImGui::GetCursorPos();
		ImVec2 cursorPos2{}; //This is for setting after all members are placed

        _curr_component = component;
        if (_prefab_inst)
        {
            _prefab_curr_component_nth = -1;
            const span comps = _prefab_inst->GetGameObject()->GetComponents();
            for (const auto& c : comps)
            {
                if (c.type == component.type)
                    ++_prefab_curr_component_nth;
                if (c == component)
                    break;
            }
        }

        ImGui::PushID("__component_header");
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
        bool open_header = ImGui::TreeNodeEx("",
            ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowItemOverlap |
            ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_SpanFullWidth);
        ImGui::PopStyleVar();

        if (ImGui::IsMouseReleased(1) && ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup))
            ImGui::OpenPopup("AdditionalOptions");

        ImGui::SameLine();
		ImGui::SetCursorPosX(ImGui::GetStyle().IndentSpacing);
		component.visit([](auto h) {
			const auto i = ComponentIcon<std::decay_t<decltype(*h)>>;
			if (*i != '\0')
			{
				ImGui::Text(i);
				ImGui::SameLine();
			}
			else
				ImGui::SetCursorPosX(ImGui::GetStyle().IndentSpacing + 12.0f + ImGui::GetStyle().ItemInnerSpacing.x * 2.0f);
			});
		auto cursor_x = ImGui::GetCursorPosX();

        if (auto f = (*component).get_property("enabled"); f.value.valid())
        {
            ImGui::Checkbox("##enabled", &f.value.get<bool>());
            ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);
        }
        else
            ImGui::SetCursorPosX(cursor_x + ImGui::GetStyle().ItemInnerSpacing.x +
                /* checkbox width: */ ImGui::GetTextLineHeight() + ImGui::GetStyle().FramePadding.y * 2);

        ImGuidk::PushFont(FontType::Bold);
        ImGui::Text(displayingComponent.c_str());
        ImGui::PopFont();

		cursorPos2 = ImGui::GetCursorPos();
        ImGui::SameLine();
		ImGui::SetCursorPos(cursorPos);
		ImGui::SetCursorPosX(ImGui::GetWindowContentRegionWidth() - ImGui::CalcTextSize("...").x - ImGui::GetStyle().FramePadding.x * 2);

        ImGui::PushStyleColor(ImGuiCol_Button, 0);
		if (ImGui::Button("..."))
			ImGui::OpenPopup("AdditionalOptions");
        ImGui::PopStyleColor();

		ImGui::SetCursorPos(cursorPos2);

		if (ImGui::BeginPopup("AdditionalOptions", ImGuiWindowFlags_None)) {
			if (ImGui::MenuItem("Reset")) {
                if (component.is_type<Transform>())
                {
                    auto t = handle_cast<Transform>(component);
                    t->position = vec3(0, 0, 0);
                    t->rotation = quat();
                    t->scale = vec3(1.0f, 1.0f, 1.0f);
                }
                else
                {
                    auto dyn = *component;
                    for (size_t i = 0; i < dyn.type.count(); ++i)
                    {
                        auto prop = dyn.get_property(i);
                        prop.value = prop.value.type.create();
                    }
                }
			}
			ImGui::Separator();
            if (!component.is_type<Transform>())
			    MenuItem_RemoveComponent(component);
			MenuItem_CopyComponent(component);
			MenuItem_PasteComponent();
			ImGui::EndPopup();
		}

        if (open_header)
        {
            component.visit([&](auto h) { DisplayComponentInner(h); });
            ImGui::TreePop();
        }

        ImGui::PopID();
        ImGui::PopID();
        ImGui::PopID();

        ImGui::Separator();
	}

	void IGE_InspectorWindow::MenuItem_RemoveComponent(GenericHandle i)
	{
		if (ImGui::MenuItem("Remove Component")) {
			IDE& editor = Core::GetSystem<IDE>();

			if (editor.selected_gameObjects.size() == 1) {
                Handle<GameObject> go = i.visit([](auto h)
                {
                    if constexpr (!std::is_same_v<decltype(h), Handle<GameObject>>)
                        return h->GetGameObject();
                    else
                        return Handle<GameObject>();
                });
                Core::GetSystem<IDE>().command_controller.ExecuteCommand(COMMAND(CMD_DeleteComponent, go, i));
			}
			else {
				int execute_counter = 0;
				for (Handle<GameObject> gameObject : editor.selected_gameObjects) {
					Core::GetSystem<IDE>().command_controller.ExecuteCommand(COMMAND(CMD_DeleteComponent, gameObject, string((*i).type.name())));
					++execute_counter;
				}
				CommandController& commandController = Core::GetSystem<IDE>().command_controller;
				commandController.ExecuteCommand(COMMAND(CMD_CallCommandAgain, execute_counter));
			}
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
			IDE& editor = Core::GetSystem<IDE>();
			if (!editor.copied_component.valid())
				return;

			bool isTransformValuesEdited = false;
			editor.RefreshSelectedMatrix();

			int execute_counter = 0;
			for (int i = 0; i < editor.selected_gameObjects.size(); ++i) {
				auto& gameObject = editor.selected_gameObjects[i];

				GenericHandle componentToMod = gameObject->GetComponent(editor.copied_component.type);
				if (componentToMod) { //Name cannot be pasted as there is no button to copy
					//replace values
					if (componentToMod == gameObject->GetComponent<Transform>()) {
						isTransformValuesEdited = true;
						vector<mat4>& originalMatrix = editor.selected_matrix;
						
						Handle<Transform> copiedTransform = handle_cast<Transform>(editor.copied_component.get<GenericHandle>());
						Handle<Transform> gameObjectTransform = editor.selected_gameObjects[i]->GetComponent<Transform>();
						if (copiedTransform->parent)
							gameObjectTransform->LocalMatrix(copiedTransform->LocalMatrix()); 
						else 
							gameObjectTransform->GlobalMatrix(copiedTransform->GlobalMatrix()); //If the parent of the copied gameobject component is deleted, use Global instead

						mat4 modifiedMat = gameObjectTransform->GlobalMatrix();
						editor.command_controller.ExecuteCommand(COMMAND(CMD_TransformGameObject, editor.selected_gameObjects[i], originalMatrix[i], modifiedMat));
						++execute_counter;
					}
					else {
						//Mark to remove Component
						string compName = string((*componentToMod).type.name());
						Core::GetSystem<IDE>().command_controller.ExecuteCommand(COMMAND(CMD_DeleteComponent, gameObject, compName));
						editor.command_controller.ExecuteCommand(COMMAND(CMD_AddComponent, gameObject, editor.copied_component)); //Remember commands are flushed at end of each update!
						execute_counter += 2;
					}
				}
				else {
					//Add component
					editor.command_controller.ExecuteCommand(COMMAND(CMD_AddComponent, gameObject, editor.copied_component));
					++execute_counter;
				}
			}
			CommandController& commandController = Core::GetSystem<IDE>().command_controller;
			commandController.ExecuteCommand(COMMAND(CMD_CallCommandAgain, execute_counter));
			if (isTransformValuesEdited)
				//Refresh the new matrix values
				editor.RefreshSelectedMatrix();
		}
	}



    // when curr property is key, draws using CustomDrawFn
    bool IGE_InspectorWindow::DisplayVal(reflect::dynamic dyn, InjectDrawTable* inject_draw_table)
    {
        const float pad_y = ImGui::GetStyle().FramePadding.y;

        static reflect::dynamic original_value;
        bool outer_changed = false;
        vector<char> indent_stack;

        auto prop_stack_copy = _curr_property_stack;

        const auto display_key_value = [&](const char* key, auto&& val, int depth_change)
        {
            using T = std::decay_t<decltype(val)>;

            const float currentHeight = ImGui::GetCursorPosY();

            string keyName = format_name(key);

            while (depth_change++ <= 0)
            {
                if (indent_stack.back())
                    ImGui::Unindent();
				if (!indent_stack.empty()) indent_stack.pop_back();
                if(!_curr_property_stack.empty()) _curr_property_stack.pop_back();
            }
            _curr_property_stack.push_back(key);

            if (keyName == "Enabled")
            {
                indent_stack.push_back(0);
                return false;
            }

            DisplayStack display(*this);
            display.GroupBegin();

            ImGui::SetCursorPosY(currentHeight + pad_y);
            display.Label(keyName.c_str());

            ImGui::SetCursorPosY(currentHeight);
            display.ItemBegin(true);

            bool indent = false;
            bool recurse = false;
            bool changed = false;
            [[maybe_unused]] bool changed_and_deactivated = false;

            //ALL THE TYPE STATEMENTS HERE
            bool draw_injected = false;
            if (inject_draw_table)
            {
                if (const auto iter = inject_draw_table->find(display.curr_prop_path); iter != inject_draw_table->end())
                {
                    changed |= iter->second(val);
                    draw_injected = true;
                }
            }
            if (!draw_injected)
            {
                if constexpr (std::is_same_v<T, float> || std::is_same_v<T, real>)
                {
                    changed |= ImGui::DragFloat("", &val, 0.01f);
                }
                else if constexpr (std::is_same_v<T, int>)
                {
                    changed |= ImGui::DragInt("", &val);
                }
                else if constexpr (std::is_same_v<T, uint32_t>)
                {
                    changed |= ImGui::DragScalar("", ImGuiDataType_U32, &val, 1.0f);
                }
                else if constexpr (std::is_same_v<T, short>)
                {
                    changed |= ImGui::DragScalar("", ImGuiDataType_S16, &val, 1.0f);
                }
                else if constexpr (std::is_same_v<T, uint16_t>)
                {
                    changed |= ImGui::DragScalar("", ImGuiDataType_U16, &val, 1.0f);
                }
                else if constexpr (std::is_same_v<T, long long>)
                {
                    changed |= ImGui::DragScalar("", ImGuiDataType_S64, &val, 1.0f);
                }
                else if constexpr (std::is_same_v<T, size_t>)
                {
                    changed |= ImGui::DragScalar("", ImGuiDataType_U64, &val, 1.0f);
                }
                else if constexpr (std::is_same_v<T, bool>)
                {
                    changed |= ImGui::Checkbox("", &val);
                }
                else if constexpr (std::is_same_v<T, vec2>)
                {
                    changed |= ImGuidk::DragVec2("", &val);
                }
                else if constexpr (std::is_same_v<T, vec3>)
                {
                    changed |= ImGuidk::DragVec3("", &val);
                }
                else if constexpr (std::is_same_v<T, quat>)
                {
                    changed |= ImGuidk::DragQuat("", &val);
                }
                else if constexpr (std::is_same_v<T, rect>)
                {
                    changed |= ImGuidk::DragRect("", &val);
                }
                else if constexpr (std::is_same_v<T, color>)
                {
                    changed |= ImGui::ColorEdit4("", val.data());
                }
                else if constexpr (std::is_same_v<T, rad>)
                {
                    changed |= ImGui::SliderAngle("", val.data());
                }
                else if constexpr (std::is_same_v<T, string>)
                {
                    changed |= ImGui::InputText("", &val);
                }
                else if constexpr (is_template_v<T, RscHandle>)
                {
                    changed |= ImGuidk::InputResource("", &val);
                }
                else if constexpr (std::is_same_v<T, Handle<GameObject>>)
                {
                    changed |= ImGuidk::InputGameObject("", &val);
                }
                else if constexpr (std::is_same_v<T, LayerMask>)
                {
                    changed |= ImGuidk::LayerMaskCombo("", &val);
                }
                else if constexpr (is_macro_enum_v<T>)
                {
                    changed |= ImGuidk::EnumCombo("", &val);
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

                    if (ImGui::Combo("##variant", &new_ind, combo_items.data(), static_cast<int>(sz)))
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
                        DisplayVal(dyn);
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
                        DisplayVal(pair[0]);
                    }
                    ImGui::Unindent();*/
                }
                else
                {
                    if (keyName.size())
                    {
                        ImGui::NewLine();
                        indent = true;
                    }
                    else
                    {
                        ImGui::SetCursorPosX(0);
                        ImGui::SetCursorPosY(currentHeight);
                        ImGui::EndGroup();
                        ImGui::PopItemWidth();
                        ImGui::PopID();
                        indent_stack.push_back(0);
                        ImGui::SetCursorPosY(currentHeight);
                        return true;
                    }
                    recurse = true;
                }
            }

            display.ItemEnd();

            outer_changed |= changed;
            display.GroupEnd(changed);

            indent_stack.push_back(indent);
            if (indent)
                ImGui::Indent();

            if (ImGui::IsItemActive() && ImGui::GetCurrentContext()->ActiveIdIsJustActivated)
            {
                original_value.swap(reflect::dynamic(val).copy());
            }
            else if (ImGui::IsItemDeactivatedAfterEdit())
            {
                Core::GetSystem<IDE>().command_controller.ExecuteCommand(
                    COMMAND(CMD_ModifyProperty, _curr_component, display.curr_prop_path, original_value, reflect::dynamic(val).copy()));
                original_value.swap(reflect::dynamic());
            }

            return recurse;
        };

        const auto generic_visitor = [&](auto&& key, auto&& val, int depth_change)
        {
            using K = std::decay_t<decltype(key)>;

            (key); (val); (depth_change);
            if constexpr (std::is_same_v<K, reflect::type>) // from variant visit
            {
                // add empty key so that can be popped properly
                return display_key_value("", std::forward<decltype(val)>(val), depth_change);
            }
            else if constexpr (!std::is_same_v<K, const char*>)
                throw "unhandled case";
            else
                return display_key_value(key, std::forward<decltype(val)>(val), depth_change);
		};

		dyn.visit(generic_visitor);
		if (dyn.is<mono::Behavior>())
			dyn.get<mono::Behavior>().GetObject().Visit(generic_visitor, _debug_mode);

        std::swap(prop_stack_copy, _curr_property_stack);
        for (auto i : indent_stack)
        {
            if (i)
                ImGui::Unindent();
        }

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


    void IGE_InspectorWindow::DisplayStack::ItemBegin(bool align)
    {
        if (align)
        {
            ImGui::SameLine();
            ImGui::SetCursorPosX(ImGui::GetWindowContentRegionWidth() * (1.0f - item_width_ratio));
        }
        ImGui::PushID(("##" + curr_prop_path).c_str());
        ImGui::PushItemWidth(-4.0f);
    }

    void IGE_InspectorWindow::DisplayStack::ItemEnd()
    {
        ImGui::PopItemWidth();
        ImGui::PopID();
    }

    void IGE_InspectorWindow::DisplayStack::GroupBegin()
    {
        curr_prop_path.clear();
        for (const auto& prop : self._curr_property_stack)
        {
            if (prop.empty())
                continue;
            curr_prop_path += prop;
            curr_prop_path += '/';
        }
        curr_prop_path.pop_back();
        
        has_override = false;
        if (self._prefab_inst && self._curr_property_stack.back().size())
            has_override = self._prefab_inst->HasOverride(
                (*self._curr_component).type.name(), curr_prop_path, self._prefab_curr_component_nth);

        ImGui::BeginGroup();
    }

    void IGE_InspectorWindow::DisplayStack::GroupEnd(bool changed)
    {
        ImGui::EndGroup();

        if (has_override && ImGui::BeginPopupContextItem("__context"))
        {
            if (ImGui::MenuItem("Apply Property"))
            {
                PropertyOverride ov{ string((*self._curr_component).type.name()), curr_prop_path, self._prefab_curr_component_nth };
                PrefabUtility::ApplyPropertyOverride(self._prefab_inst->GetGameObject(), ov);
            }
            if (ImGui::MenuItem("Revert Property"))
            {
                PropertyOverride ov{ string((*self._curr_component).type.name()), curr_prop_path, self._prefab_curr_component_nth };
                PrefabUtility::RevertPropertyOverride(self._prefab_inst->GetGameObject(), ov);
            }
            ImGui::EndPopup();
        }

        if (changed && self._prefab_inst)
            PrefabUtility::RecordPrefabInstanceChange(self._prefab_inst->GetGameObject(), self._curr_component, curr_prop_path);

        if (has_override)
        {
            ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(ImGui::GetWindowPos().x, ImGui::GetItemRectMin().y) + ImVec2(4.0f, 0),
                ImVec2(ImGui::GetWindowPos().x, ImGui::GetItemRectMin().y) + ImVec2(4.0f, 0) + ImVec2(4.0f, ImGui::GetItemRectSize().y),
                ImGui::GetColorU32(ImGuiCol_PlotLinesHovered));
        }
    }

    void IGE_InspectorWindow::DisplayStack::Label(const char* key)
    {
        if (has_override)
        {
            ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetColorU32(ImGuiCol_PlotLinesHovered));
            ImGui::Text(key);
            ImGui::PopStyleColor();
        }
        else
            ImGui::Text(key);
        ImGui::SameLine();
    }

}
