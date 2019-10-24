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
#include <editor/windows/IGE_ProjectSettings.h>
#include <editor/utils.h>
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
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	}



	void IGE_InspectorWindow::Update()
	{
		ImGui::PopStyleVar(2);

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

        if (gameObjectsCount == 0)
            return;

        // HEADER
        {
            ImGui::GetWindowDrawList()->ChannelsSplit(2);
            ImGui::GetWindowDrawList()->ChannelsSetCurrent(1);

            ImGui::BeginGroup();
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 8.0f);

            DisplayGameObjectHeader(gos[0]);

            if (const auto prefab_inst = gos[0]->GetComponent<PrefabInstance>())
            {
                _prefab_inst = prefab_inst;
                if (prefab_inst->object_index == 0)
                    DisplayPrefabInstanceControls(_prefab_inst);
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
        Handle<Transform> c_transform = gos[0]->GetComponent<Transform>();
        if (c_transform) {
            DisplayComponent(c_transform);
        }

        if (gameObjectsCount == 1)
        {
            //Display remaining components here
            auto componentSpan = gos[0]->GetComponents();
            for (auto& component : componentSpan) {

                //Skip Name and Transform and PrefabInstance
				if (component == c_transform ||
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
                                          ImGuiTreeNodeFlags_SpanAllAvailWidth);

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

		if (ImGui::BeginPopup("AddScript", ImGuiWindowFlags_None)) {
			auto* script_env = &Core::GetSystem<mono::ScriptSystem>().ScriptEnvironment();
			if (script_env == nullptr)
				ImGui::Text("Scripts not loaded!");

			span componentNames = script_env->GetBehaviorList();
			for (const char* name : componentNames) {
				if (ImGui::MenuItem(name)) {
					for (Handle<GameObject> i : gos)
						Core::GetSystem<IDE>().command_controller.ExecuteCommand(COMMAND(CMD_AddBehavior, i, string{ name }));
				}
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
        ImGui::PopItemWidth();


		if (ImGui::IsItemClicked()) {
			stringBuf = game_object->Name();
		}
		else if (!ImGui::IsItemActive()) { //Disable assignment when editing text
			stringBuf = game_object->Name();
		}

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

    template<>
	void IGE_InspectorWindow::DisplayComponentInner(Handle<Animator> c_anim)
	{
		ImVec2 cursorPos = ImGui::GetCursorPos();
		ImVec2 cursorPos2{};

		//Draw All your custom variables here.

		ImGui::Text("Animation Clips");
		ImGui::Separator();
		for (auto& anim : c_anim->animation_table)
		{
			bool not_removed = true;
			// Will change this to use something other than collapsing header. 
			if(ImGui::CollapsingHeader(anim.second.animation->Name().data(), &not_removed, ImGuiTreeNodeFlags_AllowItemOverlap))
			{
				ImGui::Indent(50);
				ImGui::PushItemWidth(100);
				ImGui::DragFloat("Speed", &anim.second.speed, 0.01f, 0.0f);
				ImGui::PopItemWidth();
				ImGui::Checkbox("Loop", &anim.second.loop);
				ImGui::Unindent(50);
			}

			if (!not_removed)
			{
				c_anim->RemoveAnimation(anim.second.name);
				break;
			}
		}


		RscHandle<anim::Animation> new_anim;
		if (ImGuidk::InputResource("Add Animation Clip", &new_anim))
		{
			c_anim->AddAnimation(new_anim);
		}
		ImGui::NewLine();

		if (ImGui::BeginCombo("Default State", c_anim->GetAnimationState(c_anim->layers[0].default_state).name.c_str()))
		{
			for (auto& anim : c_anim->animation_table)
			{
				string_view curr_name = anim.second.name;
				if (ImGui::Selectable(curr_name.data(), c_anim->layers[0].default_state == curr_name))
				{
					// c_anim->Stop();
					c_anim->layers[0].default_state = curr_name;
					if (c_anim->layers[0].curr_state == string{})
						c_anim->layers[0].curr_state = curr_name;
				}
			}
			ImGui::EndCombo();
		}

		if (ImGui::BeginCombo("Current State", c_anim->GetAnimationState(c_anim->layers[0].curr_state).name.c_str()))
		{
			for (auto& anim : c_anim->animation_table)
			{
				string_view curr_name = anim.second.name;
				if (ImGui::Selectable(curr_name.data(), c_anim->layers[0].curr_state == curr_name))
				{
					// Reset the animation
					c_anim->layers[0].normalized_time = 0.0f;
					// Set the new current animation
					c_anim->layers[0].curr_state = curr_name;
				}
			}
			ImGui::EndCombo();
		}
		bool has_curr_anim = c_anim->layers[0].curr_state != string{};
		if (has_curr_anim)
		{
			if (ImGui::Checkbox("Preview Current State", &c_anim->layers[0].preview_playback))
			{
				if (!c_anim->layers[0].preview_playback)
				{
					c_anim->layers[0].normalized_time = 0.0f;
                    Core::GetSystem<AnimationSystem>().RestoreBindPose(*c_anim);
				}
			}

			ImGui::ProgressBar(c_anim->layers[0].normalized_time, ImVec2{ -1, 10 }, nullptr);
		}
			
		// FOR TESTING 
		ImGui::Text("TESTING");
		ImGui::Separator();

	    if (ImGui::Button("Play"))
	    {
		    if(!c_anim->animation_table.empty())
			    c_anim->Play(c_anim->animation_table.begin()->second.name);
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

        ImGui::DragFloat("Test Blend", &c_anim->layers[0].blend_time, 0.01f, 0.0f, 1.0f);
	}

    template<>
	void IGE_InspectorWindow::DisplayComponentInner(Handle<Bone> c_bone)
	{
		//Draw All your custom variables here.
		ImGui::Text("Bone Name: ");
		ImGui::SameLine();
		ImGui::Text(c_bone->_bone_name.c_str());
		ImGui::Text("Bone Index: %d", c_bone->_bone_index);
	}

	void IGE_InspectorWindow::DisplayComponent(GenericHandle& component)
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

        if (_prefab_inst)
        {
            _prefab_curr_component = component;
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
            ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_SpanAllAvailWidth);
        ImGui::PopStyleVar();

        if (ImGui::IsMouseReleased(1) && ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup))
            ImGui::OpenPopup("AdditionalOptions");

        ImGui::SameLine();
        if (auto f = (*component).get_property("enabled"); f.value.valid())
        {
            ImGui::SetCursorPosX(ImGui::GetStyle().IndentSpacing);
            ImGui::Checkbox("##enabled", &f.value.get<bool>());
            ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);
        }
        else
            ImGui::SetCursorPosX(ImGui::GetStyle().IndentSpacing + ImGui::GetStyle().ItemInnerSpacing.x +
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
            if (component.is_type<Transform>())
                DisplayComponentInner(handle_cast<Transform>(component));
            else if (component.is_type<Bone>())
                DisplayComponentInner(handle_cast<Bone>(component));
            else if (component.is_type<Animator>())
                DisplayComponentInner(handle_cast<Animator>(component));
            else
                displayVal(*component);
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
			for (Handle<GameObject> gameObject : editor.selected_gameObjects)
				Core::GetSystem<IDE>().command_controller.ExecuteCommand(COMMAND(CMD_DeleteComponent, gameObject, string((*i).type.name())));

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

					}
					else {
						//Mark to remove Component
						string compName = string((*componentToMod).type.name());
						Core::GetSystem<IDE>().command_controller.ExecuteCommand(COMMAND(CMD_DeleteComponent, gameObject, compName));
						editor.command_controller.ExecuteCommand(COMMAND(CMD_AddComponent, gameObject, editor.copied_component)); //Remember commands are flushed at end of each update!
					}
				}
				else {
					//Add component
					editor.command_controller.ExecuteCommand(COMMAND(CMD_AddComponent, gameObject, editor.copied_component));
				}
			}

			if (isTransformValuesEdited)
				//Refresh the new matrix values
				editor.RefreshSelectedMatrix();
		}
	}
	static void DoNothing() {}


    bool IGE_InspectorWindow::displayVal(reflect::dynamic dyn)
    {
        const float item_width = ImGui::GetWindowContentRegionWidth() * item_width_ratio;
        const float pad_y = ImGui::GetStyle().FramePadding.y;

        bool outer_changed = false;
        vector<char> indent_stack;

        const auto generic_visitor = [&](auto&& key, auto&& val, int depth_change) { //Displays all the members for that variable

            using K = std::decay_t<decltype(key)>;
            using T = std::decay_t<decltype(val)>;

            //reflect::dynamic dynaVal = val;
            const float currentHeight = ImGui::GetCursorPosY();

            if constexpr (std::is_same_v<K, reflect::type>) // from variant visit
            {
                // add empty key so that can be popped properly
                _curr_property_stack.push_back("");
                indent_stack.push_back(0);
                return true;
            }
            else if constexpr (!std::is_same_v<K, const char*>)
                throw "unhandled case";
            else
            {
                string keyName = format_name(key);
				if (keyName == "min" || keyName == "Min")
				{
					DoNothing();
				}
                while (depth_change++ <= 0)
                {
                    if (indent_stack.back())
                        ImGui::Unindent();
                    indent_stack.pop_back();
                    _curr_property_stack.pop_back();
                }
                _curr_property_stack.push_back(key);

                if (keyName == "Enabled")
                {
                    indent_stack.push_back(0);
                    return false;
                }

                string curr_prop_path;
                for (const auto& prop : _curr_property_stack)
                {
                    if (prop.empty())
                        continue;
                    curr_prop_path += prop;
                    curr_prop_path += '/';
                }
                curr_prop_path.pop_back();

                bool has_override = false;
                if (_prefab_inst)
                {
                    for (const auto& ov : _prefab_inst->overrides)
                    {
                        if (ov.component_name == (*_prefab_curr_component).type.name() &&
                            ov.property_path == curr_prop_path &&
                            ov.component_nth == _prefab_curr_component_nth)
                        {
                            has_override = true;
                            break;
                        }
                    }
                }

                ImGui::BeginGroup();

                ImGui::SetCursorPosY(currentHeight + pad_y);
                if (has_override)
                {
                    ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetColorU32(ImGuiCol_PlotLinesHovered));
                    ImGui::Text(keyName.c_str());
                    ImGui::PopStyleColor();
                    ImGui::SameLine(-ImGui::GetStyle().IndentSpacing);
                    ImGui::GetWindowDrawList()->AddRectFilled(ImGui::GetCursorScreenPos(),
                                                              ImGui::GetCursorScreenPos() + ImVec2(4.0f, ImGui::GetFrameHeight()),
                                                              ImGui::GetColorU32(ImGuiCol_PlotLinesHovered));
                }
                else
                    ImGui::Text(keyName.c_str());

                keyName.insert(0, "##"); //For Imgui stuff

                ImGui::SetCursorPosY(currentHeight);
                ImGui::SetCursorPosX(ImGui::GetWindowContentRegionWidth() - item_width);

                ImGui::PushID(("##"+curr_prop_path).c_str());
                ImGui::PushItemWidth(-4.0f);

                bool indent = false;
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
                    indent = true;
                    recurse = true;
                    /*ImGui::SetCursorPosY(currentHeight + heightOffset);
                    ImGui::TextDisabled("Member type not defined in IGE_InspectorWindow::Update");*/
                }

                ImGui::EndGroup();

                if (has_override && ImGui::BeginPopupContextItem("__context"))
                {
                    if (ImGui::MenuItem("Apply Property"))
                    {
                        PropertyOverride ov{ string((*_prefab_curr_component).type.name()), curr_prop_path, _prefab_curr_component_nth };
                        PrefabUtility::ApplyPropertyOverride(_prefab_inst->GetGameObject(), ov);
                    }
                    if (ImGui::MenuItem("Revert Property"))
                    {
                        PropertyOverride ov{ string((*_prefab_curr_component).type.name()), curr_prop_path, _prefab_curr_component_nth };
                        PrefabUtility::RevertPropertyOverride(_prefab_inst->GetGameObject(), ov);
                    }
                    ImGui::EndPopup();
                }

                outer_changed |= changed;
                if (changed && _prefab_inst)
                    PrefabUtility::RecordPrefabInstanceChange(_prefab_inst->GetGameObject(), _prefab_curr_component, curr_prop_path);

                ImGui::PopItemWidth();
                ImGui::PopID();

                indent_stack.push_back(indent);
                if (indent)
                    ImGui::Indent();

                return recurse;
            }

		};

		dyn.visit(generic_visitor);
		if (dyn.is<mono::Behavior>())
			dyn.get<mono::Behavior>().GetObject().Visit(generic_visitor);

        _curr_property_stack.clear();

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
