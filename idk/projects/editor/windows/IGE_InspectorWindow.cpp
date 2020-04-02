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
#include <editor/SceneManagement.h>

#include <common/TagManager.h>
#include <common/LayerManager.h>
#include <anim/AnimationSystem.h>
#include <anim/Animator.inl>
#include <app/Application.h>
#include <ds/span.h>
#include <reflect/reflect.inl>
#include <res/ResourceHandle.inl>
#include <res/ResourceManager.inl>
#include <res/ResourceHandle.inl>
#include <res/ResourceUtils.inl>
#include <res/ResourceMeta.inl>
#include <scene/SceneManager.h>
#include <math/euler_angles.h>
#include <meta/variant.h>
#include <script/MonoBehaviorEnvironment.h>
#include <prefab/PrefabUtility.h>
#include <core/Handle.inl>
#include <script/ScriptSystem.h>
#include <script/ManagedObj.inl>
#include <serialize/text.inl>
#include <util/property_path.h>

#include <IncludeComponents.h>
#include <IncludeResources.h>

#include <imgui/imgui_stl.h>
#include <imgui/imgui_internal.h> //InputTextEx
#include <iostream>

#include <gfx/GraphicsSystem.h>
#include <ds/span.inl>
#include <ds/result.inl>


#include <res/ResourceMeta.inl>

namespace idk {

	IGE_InspectorWindow::IGE_InspectorWindow()
		:IGE_IWindow{ "Inspector##IGE_InspectorWindow",true,ImVec2{ 300,600 },ImVec2{ 450,150 } } 
	{	//Delegate Constructor to set window size
		// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.
        window_flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar;
		/*Core::GetGameState().OnObjectDestroy<GameObject>() += [this](Handle<GameObject> go)
		{
			bool die = false;
			for (auto& elem : _prefab_store)
			{
				if (elem.second == go)
				{
					die = true;
					break;
				}
			}
			if (die)
				_prefab_store.clear();
		};*/
	}

    void IGE_InspectorWindow::Initialize()
    {
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

        auto& ide = Core::GetSystem<IDE>();
        const auto& selection = ide.GetSelectedObjects();

        // reset mocked particle system to default state
        if (_mocked_ps)
        {
            if(selection.game_objects.empty() || _mocked_ps->GetGameObject() != selection.game_objects[0])
            {
                _mocked_ps->Stop();
                _mocked_ps->state = ParticleSystem::Awake;
                _mocked_ps = {};
            }
        }

        if (selection.game_objects.empty() && selection.assets.empty())
            return;
        if (selection.game_objects.size() && selection.assets.size())
            return;

        _curr_component = {};

        if (selection.game_objects.size())
        {
            DisplayGameObjects(selection.game_objects);
        }
        else
        {
            const bool valid = std::visit([](auto h) { return bool(h); }, selection.assets[0]);
            if (valid)
                DisplayAsset(selection.assets[0]);
        }
	}

	void IGE_InspectorWindow::Reset()
	{
		_prefab_store.clear();
	}

    void IGE_InspectorWindow::DisplayGameObjects(vector<Handle<GameObject>> gos)
    {
        gos.erase(std::remove_if(gos.begin(), gos.end(), [](auto h) { return !h; }), gos.end());

        const size_t gameObjectsCount = gos.size();

        if (gameObjectsCount == 0)
            return;

        // HEADER
        {
            ImGui::GetWindowDrawList()->ChannelsSplit(2);
            ImGui::GetWindowDrawList()->ChannelsSetCurrent(1);

            ImGui::BeginGroup();
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 8.0f);

            if (gos[0].scene == Scene::prefab)
            {
                if (const auto prefab_inst = gos[0]->GetComponent<PrefabInstance>())
                {
                    if (prefab_inst->prefab)
                        _prefab_inst = gos[0]->GetComponent<PrefabInstance>();
                }
                DisplayGameObjectHeader(gos[0]);
            }
            else if (const auto prefab_inst = gos[0]->GetComponent<PrefabInstance>())
            {
                if (prefab_inst->prefab)
                    _prefab_inst = prefab_inst;
                DisplayGameObjectHeader(gos[0]);
                if (prefab_inst->object_index == 0)
                    DisplayPrefabInstanceControls(_prefab_inst);
            }
            else
                DisplayGameObjectHeader(gos[0]);

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
        {
            if (_debug_mode)
                ImGui::Text("%lld (scene: %d, index: %d, gen: %d)",
                    rect_transform.id,
                    s_cast<int>(rect_transform.scene),
                    s_cast<int>(rect_transform.index),
                    s_cast<int>(rect_transform.gen));
            DisplayComponent(rect_transform);
        }
        else
        {
            const auto transform = gos[0]->GetComponent<Transform>();
            if (_debug_mode)
                ImGui::Text("%lld (scene: %d, index: %d, gen: %d)",
                    transform.id,
                    s_cast<int>(transform.scene),
                    s_cast<int>(transform.index),
                    s_cast<int>(transform.gen));
            DisplayComponent(transform);
        }

        if (gameObjectsCount == 1)
        {
            auto componentSpan = gos[0]->GetComponents();
            if (!_prefab_inst)
            {
                //Display remaining components here
                for (auto& component : componentSpan) 
                {

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
                    if (_debug_mode)
                        ImGui::Text("%lld (scene: %d, index: %d, gen: %d)",
                            component.id,
                            s_cast<int>(component.scene),
                            s_cast<int>(component.index),
                            s_cast<int>(component.gen));
                    DisplayComponent(component);
                }
            }
            else
            {
                vector<int> removed, added;
                PrefabUtility::GetPrefabInstanceComponentDiff(gos[0], removed, added);
                auto prefab_components = _prefab_inst->prefab->data[_prefab_inst->object_index].components;

                for (int i = 0, j = 0; i < prefab_components.size() || j < componentSpan.size(); ++i)
                {
                    if (std::find(removed.begin(), removed.end(), i) != removed.end()) // i is removed
                    {
                        // Draw Removed Component
                        ImGui::PushID(i);

                        ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetColorU32(ImGuiCol_TextDisabled));
                        ImGui::TreeNodeEx("", ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_Bullet |
                                            ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_AllowItemOverlap |
                                            ImGuiTreeNodeFlags_SpanFullWidth);
                        ImGui::PopStyleColor();

                        string_view name = prefab_components[i].type.name();
                        if (prefab_components[i].type.is<mono::Behavior>())
                            name = prefab_components[i].get<mono::Behavior>().TypeName();

                        bool will_break = false;
                        if (ImGui::BeginPopupContextItem())
                        {
                            if (ImGui::MenuItem("Revert Removed Component"))
                            {
                                PrefabUtility::RevertRemovedComponent(gos[0], i);
                                will_break = true;
                            }
                            if (ImGui::MenuItem("Apply Removed Component"))
                            {
                                PrefabUtility::RemoveComponentFromPrefab(_prefab_inst->prefab, _prefab_inst->object_index, i);
                                will_break = true;
                            }
                            ImGui::EndPopup();
                        }

                        ImGui::SameLine(ImGui::GetStyle().IndentSpacing +
                                        ImGui::GetStyle().FramePadding.y * 2 + ImGui::GetTextLineHeight() + 1);
                        ImGuidk::PushFont(FontType::Bold);
                        ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetColorU32(ImGuiCol_TextDisabled));
                        ImGui::Text("%s (Removed)", name.data());
                        ImGui::PopStyleColor();
                        ImGui::PopFont();

                        ImGui::PopID();

                        if (will_break)
                            break;

                        continue;
                    }

                    if (j >= componentSpan.size())
                        continue;
                    auto& component = componentSpan[j];

                    //Skip Name and Transform and PrefabInstance
                    if (component.is_type<Transform>() ||
                        component.is_type<RectTransform>() ||
                        component.is_type<PrefabInstance>() ||
                        component.is_type<Name>() ||
                        component.is_type<Tag>() ||
                        component.is_type<Layer>()
                        )
                    {
                        ++j;
                        if (component.is_type<PrefabInstance>())
                            --i; // PrefabInstance will never be in prefab_data, so prefab_data has 1 less component
                        continue;
                    }

                    _curr_component_is_added = std::find(added.begin(), added.end(), j) != added.end(); // j is newly added

                    if (_debug_mode)
                        ImGui::Text("%lld (scene: %d, index: %d, gen: %d)",
                            component.id,
                            s_cast<int>(component.scene),
                            s_cast<int>(component.index),
                            s_cast<int>(component.gen));
                    DisplayComponent(component);
                    ++j;
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
                    component.is_type<RectTransform>() ||
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
					int execute_counter = 0;
                    for (Handle<GameObject> i : gos)
                    {
                        Core::GetSystem<IDE>().ExecuteCommand<CMD_AddComponent>(i, string{ name });
						++execute_counter;
                    }
                    Core::GetSystem<IDE>().ExecuteCommand<CMD_CollateCommands>(execute_counter);
                }
            }
            ImGui::EndPopup();
        }

		if (ImGui::BeginPopup("AddScript", ImGuiWindowFlags_None)) 
        {
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
			for (const char* name : componentNames) 
            {
				if (!script_textFilter.PassFilter(name)) //skip if filtered
					continue;

				if (ImGui::MenuItem(name))
                {
					for (Handle<GameObject> i : gos) 
                    {
						Core::GetSystem<IDE>().ExecuteCommand<CMD_AddBehavior>(i, string{ name });
						++execute_counter;
					}
				}
			}

			if (execute_counter > 0)
                Core::GetSystem<IDE>().ExecuteCommand<CMD_CollateCommands>(execute_counter);

			ImGui::EndPopup();
		}

        ImGui::EndChild();
    }



    template<typename T>
    auto SelectGO(Handle<GameObject>, T&& arg)
    {
        return std::forward<T>(arg);
    }
    auto SelectGO(Handle<GameObject> go, Handle<GameObject>)
    {
        return go;
    }
    template<typename Command,typename ...Args>
    void IGE_InspectorWindow::ExecuteOnSelected(Args&&... args)
    {
        IDE& editor = Core::GetSystem<IDE>();
        int execute_counter = 0;

        if (editor.GetSelectedObjects().game_objects.size())
        {
            for (auto go : editor.GetSelectedObjects().game_objects)
            {
                if (!go)
                    continue;
                editor.ExecuteCommand<Command>(SelectGO(go, std::forward<Args>(args))...);
                ++execute_counter;
            }
        }
        else
        {
            editor.ExecuteCommand<Command>(std::forward<Args>(args)...);
            ++execute_counter;
        }

        if (execute_counter > 1)
            editor.ExecuteCommand<CMD_CollateCommands>(execute_counter);
    }
//#pragma optimize("",off)

    template<typename T>
    struct has_meta
    {
        template<typename U, typename = decltype(std::declval<U>().GetMeta())>
        static char check(U&&);
        template<typename ...Args>
        static int check(Args&&...);

        inline static constexpr bool value = sizeof(check(std::declval<T>()))==1;
    };

    template<typename T>
    inline constexpr bool has_meta_v = has_meta<T>::value;

    void IGE_InspectorWindow::StoreOriginalValues(string_view property_path)
    {
        const auto& sel = Core::GetSystem<IDE>().GetSelectedObjects();
        _original_values.clear();

        if (sel.game_objects.size())
        {
            for (auto obj : sel.game_objects)
            {
                auto components = obj->GetComponents();
                auto nth = _curr_component_nth;
                for (auto c : components)
                {
                    if (c.type == _curr_component.type && nth-- == 0)
                    {
                        _original_values.push_back(resolve_property_path(*c, property_path).copy());
                        break;
                    }
                }
                if (nth >= 0) // component not found
                    _original_values.emplace_back();
            }
        }
        else if (_curr_component) // showing prefab directly
        {
            _original_values.push_back(resolve_property_path(*_curr_component, property_path).copy());
        }
        else // modifying asset meta
        {
            std::visit([&](auto h)
            {
                if constexpr (has_tag_v<decltype(h)::Resource, MetaResource>)
                    _original_values.push_back(resolve_property_path(h->GetMeta(), property_path).copy());
                else
                    _original_values.emplace_back();
            }, sel.assets[0]);
        }
    }

    void IGE_InspectorWindow::ExecuteModify(string_view property_path, reflect::dynamic new_value)
    {
        if (_original_values.empty()) // no change
            return;

        const auto& sel = Core::GetSystem<IDE>().GetSelectedObjects();

        if (sel.game_objects.size())
        {
            int execute_counter = 0;
            const auto& sel_obj = Core::GetSystem<IDE>().GetSelectedObjects().game_objects;

            // execute for similar components, i == 0 is the current displaying component
            for (size_t i = 1; i < sel_obj.size(); ++i)
            {
                auto obj = sel_obj[i];

                auto components = obj->GetComponents();
                auto nth = _curr_component_nth;
                for (auto c : components)
                {
                    if (c.type == _curr_component.type && nth-- == 0)
                    {
                        Core::GetSystem<IDE>().ExecuteCommand<CMD_ModifyProperty>(
                            c, property_path, _original_values[i], new_value);
                        ++execute_counter;
                        break;
                    }
                }
            }

            Core::GetSystem<IDE>().ExecuteCommand<CMD_ModifyProperty>(
                _curr_component, property_path, _original_values[0], new_value);
            ++execute_counter;
            if (execute_counter > 1)
                Core::GetSystem<IDE>().ExecuteCommand<CMD_CollateCommands>(execute_counter);
        }
        else if (_curr_component) // displaying prefab game object
        {
            Core::GetSystem<IDE>().ExecuteCommand<CMD_ModifyProperty>(_curr_component, property_path, _original_values[0], new_value);
        }
        else // modifying asset meta
        {
            std::visit([&](auto h)
            {
                if constexpr (has_tag_v<decltype(h)::Resource, MetaResource>)
                {
                    auto dyn = resolve_property_path(h->GetMeta(), property_path);
                    Core::GetSystem<IDE>().ExecuteCommand<CMD_ModifyProperty>(dyn, _original_values[0]);
                }
            }, sel.assets[0]);
        }

        _original_values.clear();
    }



	void IGE_InspectorWindow::DisplayGameObjectHeader(Handle<GameObject> game_object)
	{
        const float left_offset = 40.0f;

		//The c_name is to just get the first gameobject
		static std::string stringBuf{};
		IDE& editor = Core::GetSystem<IDE>();
		//ImVec2 startScreenPos = ImGui::GetCursorScreenPos();

        ImGui::SetCursorPosX(left_offset - ImGui::GetFrameHeight() - ImGui::GetStyle().FramePadding.y * 2);
        bool is_active = game_object->ActiveSelf();
        if (ImGui::Checkbox("##Active", &is_active))
            editor.ExecuteCommand<CMD_ModifyGameObjectHeader>(game_object, std::nullopt, std::nullopt, std::nullopt, is_active);
		ImGui::SameLine();
        ImGui::PushItemWidth(-8.0f);
        if (game_object.scene == Scene::prefab)
            ImGuidk::PushDisabled();

        bool has_override = false;
        if (_prefab_inst)
            has_override = _prefab_inst->HasOverride("Name", "name", 0);

        if (has_override)
            ImGuidk::PushFont(FontType::Bold);
		if (ImGui::InputText("##Name", &stringBuf, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_NoUndoRedo)) 
        {
			int execute_counter = 0;
            int i = 0;
			for (auto go : editor.GetSelectedObjects().game_objects)
            {
                if (!go)
                    continue;

				string outputString = stringBuf;
				if (i > 0) {
					outputString.append(" (");
					outputString.append(serialize_text(i));
					outputString.append(")");
				}

                editor.ExecuteCommand<CMD_ModifyGameObjectHeader>(go, outputString);
				++execute_counter;
			}

            editor.ExecuteCommand<CMD_CollateCommands>(execute_counter);
		}
        if (has_override)
        {
            ImGui::PopFont();
            ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(ImGui::GetWindowPos().x, ImGui::GetItemRectMin().y) + ImVec2(4.0f, 0),
                ImVec2(ImGui::GetWindowPos().x, ImGui::GetItemRectMin().y) + ImVec2(4.0f, 0) + ImVec2(4.0f, ImGui::GetItemRectSize().y),
                ImGui::GetColorU32(ImGuiCol_PlotLinesHovered));
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
            if (editor.GetSelectedObjects().game_objects.size() == 1)
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

        if (_prefab_inst)
            has_override = _prefab_inst->HasOverride("Tag", "", 0);
        if (has_override)
            ImGuidk::PushFont(FontType::Bold);

        ImGui::PushItemWidth(ImGui::GetWindowContentRegionWidth() * 0.5f - ImGui::GetCursorPosX());
        const auto curr_tag = game_object->Tag();
        if (ImGui::BeginCombo("##tag", curr_tag.size() ? curr_tag.data() : "Untagged"))
        {
            if (ImGui::MenuItem("Untagged"))
                game_object->Tag("");
            for (const auto& tag : Core::GetSystem<TagManager>().GetConfig().tags)
            {
                if (ImGui::MenuItem(tag.data()))
                    ExecuteOnSelected<CMD_ModifyGameObjectHeader>(game_object, std::nullopt, tag);
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
        
        if (has_override)
        {
            ImGui::PopFont();
            ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(ImGui::GetWindowPos().x, ImGui::GetItemRectMin().y) + ImVec2(4.0f, 0),
                ImVec2(ImGui::GetWindowPos().x, ImGui::GetItemRectMin().y) + ImVec2(4.0f, 0) + ImVec2(4.0f, ImGui::GetItemRectSize().y),
                ImGui::GetColorU32(ImGuiCol_PlotLinesHovered));
        }

        ImGui::Text("Layer");
        ImGui::SameLine();

        if (_prefab_inst)
            has_override = _prefab_inst->HasOverride("Layer", "index", 0);
        if (has_override)
            ImGuidk::PushFont(FontType::Bold);

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
                {
                    ExecuteOnSelected<CMD_ModifyGameObjectHeader>(game_object, std::nullopt, std::nullopt, i);
                }
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Add Layer##_add_layer_"))
            {
                Core::GetSystem<IDE>().FindWindow<IGE_ProjectSettings>()->FocusConfig<TagManager>();
            }
            ImGui::EndCombo();
        }
        ImGui::PopItemWidth();
        
        if (has_override)
        {
            ImGui::PopFont();
            ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(ImGui::GetWindowPos().x, ImGui::GetItemRectMin().y) + ImVec2(4.0f, 0),
                ImVec2(ImGui::GetWindowPos().x, ImGui::GetItemRectMin().y) + ImVec2(4.0f, 0) + ImVec2(4.0f, ImGui::GetItemRectSize().y),
                ImGui::GetColorU32(ImGuiCol_PlotLinesHovered));
        }
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
        ImGui::SameLine();
        if (ImGui::Button("Break"))
        {
            PrefabUtility::BreakPrefabInstance(c_prefab->GetGameObject());
        }
    }

	void IGE_InspectorWindow::DisplayComponent(GenericHandle component)
	{
		//COMPONENT DISPLAY
        ImGui::PushID(component.type);
        ImGui::PushID(component.index);
		string displayingComponent = [&]() ->string
		{
			auto type = (*component).type;
            if (type.is<mono::Behavior>())
            {
                auto mb = handle_cast<mono::Behavior>(component);
                auto behavior_name = string{ mb->TypeName() };
                if (mb->script_data)
                    behavior_name += " (Script)";
                else
                    behavior_name += " (Missing Script)";
                return behavior_name;
            }
            else
                return string{ type.name() };
		}();
		const string fluffText{ "idk::" };
		std::size_t found = displayingComponent.find(fluffText);

		if (found != std::string::npos)
			displayingComponent.erase(found, fluffText.size());
        if (_curr_component_is_added)
            displayingComponent += " (Added)";

		ImVec2 cursorPos = ImGui::GetCursorPos();
		ImVec2 cursorPos2{}; //This is for setting after all members are placed

        _curr_component = component;
        _curr_component_nth = -1;
        Handle<GameObject> go = component.visit([](auto h)
        {
            if constexpr (!std::is_same_v<decltype(h), Handle<GameObject>>)
                return h->GetGameObject();
            else
                return Handle<GameObject>();
        });
        for (const auto& c : go->GetComponents())
        {
            if (c.type == component.type)
                ++_curr_component_nth;
            if (c == component)
                break;
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
		if (ImGui::MenuItem("Remove Component")) 
        {
			IDE& editor = Core::GetSystem<IDE>();

			if (editor.GetSelectedObjects().game_objects.size() == 1)
            {
                Handle<GameObject> go = i.visit([](auto h)
                {
                    if constexpr (!std::is_same_v<decltype(h), Handle<GameObject>>)
                        return h->GetGameObject();
                    else
                        return Handle<GameObject>();
                });
                editor.ExecuteCommand<CMD_DeleteComponent>(go, i);
			}
			else 
            {
				int execute_counter = 0;
				for (auto go : editor.GetSelectedObjects().game_objects) 
                {
					Core::GetSystem<IDE>().ExecuteCommand<CMD_DeleteComponent>(go, string((*i).type.name()));
					++execute_counter;
				}
                editor.ExecuteCommand<CMD_CollateCommands>(execute_counter);
			}
		}
	}

	void IGE_InspectorWindow::MenuItem_CopyComponent(GenericHandle i)
	{
		if (ImGui::MenuItem("Copy Component"))
			_copied_component.swap((*i).copy());
	}

	void IGE_InspectorWindow::MenuItem_PasteComponent()
	{
		if (ImGui::MenuItem("Paste Component")) 
        {
			IDE& editor = Core::GetSystem<IDE>();
			if (!_copied_component.valid())
				return;

			int execute_counter = 0;
			for (int i = 0; i < editor.GetSelectedObjects().game_objects.size(); ++i) 
            {
                auto& gameObject = editor.GetSelectedObjects().game_objects[i];

				GenericHandle componentToMod = gameObject->GetComponent(_copied_component.type);
				if (componentToMod) //Name cannot be pasted as there is no button to copy
                {
					//replace values
					if (componentToMod == gameObject->GetComponent<Transform>()) 
                    {
						mat4 originalMatrix = gameObject->GetComponent<Transform>()->GlobalMatrix();
						
                        const auto& copiedTransform = _copied_component.get<Transform>();
						Handle<Transform> gameObjectTransform = gameObject->GetComponent<Transform>();
						if (copiedTransform.parent)
							gameObjectTransform->LocalMatrix(copiedTransform.LocalMatrix()); 
						else //If the parent of the copied gameobject component is deleted, use Global instead
							gameObjectTransform->GlobalMatrix(copiedTransform.GlobalMatrix());

						mat4 modifiedMat = gameObjectTransform->GlobalMatrix();
						editor.ExecuteCommand<CMD_TransformGameObject>(gameObject, originalMatrix, modifiedMat);
						++execute_counter;
					}
					else 
                    {
						//Mark to remove Component
						string compName = string((*componentToMod).type.name());
						Core::GetSystem<IDE>().ExecuteCommand<CMD_DeleteComponent>(gameObject, compName);
						editor.ExecuteCommand<CMD_AddComponent>(gameObject, _copied_component); //Remember commands are flushed at end of each update!
						execute_counter += 2;
					}
				}
				else 
                {
					//Add component
					editor.ExecuteCommand<CMD_AddComponent>(gameObject, _copied_component);
					++execute_counter;
				}
			}

            editor.ExecuteCommand<CMD_CollateCommands>(execute_counter);
		}
	}



    // when curr property is key, draws using CustomDrawFn
    bool IGE_InspectorWindow::DisplayVal(reflect::dynamic dyn, InjectDrawTable* inject_draw_table)
    {
        const float pad_y = ImGui::GetStyle().FramePadding.y;

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
                if (indent_stack.size())
                {
                    if (indent_stack.back())
                        ImGui::Unindent();
                    indent_stack.pop_back();
                }
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
            bool changed_and_deactivated = false;

            //ALL THE TYPE STATEMENTS HERE
            bool draw_injected = false;
            if (inject_draw_table)
            {
                if (const auto iter = inject_draw_table->find(display.curr_prop_path); iter != inject_draw_table->end())
                {
                    auto copy = val;
                    auto edit_state = iter->second(copy);
                    if (edit_state == EditState::Editing)
                    {
                        changed = true;
                        val = copy;
                    }
                    else if (edit_state == EditState::Completed)
                    {
                        changed_and_deactivated = true;
                        val = copy;
                    }
                    else if (edit_state == EditState::CompletedInOneFrame)
                    {
                        StoreOriginalValues(display.curr_prop_path);
                        val = copy;
                        changed_and_deactivated = true;
                    }

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
                    static std::string buf;
                    buf = val;
                    if (ImGui::InputText("", &buf))
                    {
                        val = buf;
                        changed = true;
                    }
                }
                else if constexpr (is_template_v<T, RscHandle>)
                {
                    auto copy = val;
                    changed |= ImGuidk::InputResource("", &copy);
                    if (changed)
                    {
                        StoreOriginalValues(display.curr_prop_path);
                        val = copy;
                        changed_and_deactivated = true;
                    }
                }
                else if constexpr (std::is_same_v<T, Handle<GameObject>>)
                {
                    auto copy = val;
                    changed |= ImGuidk::InputGameObject("", &copy);
                    if (changed)
                    {
                        StoreOriginalValues(display.curr_prop_path);
                        val = copy;
                        changed_and_deactivated = true;
                    }
                }
                else if constexpr (std::is_same_v<T, LayerMask>)
                {
                    changed |= ImGuidk::LayerMaskCombo("", &val);
                }
                else if constexpr (is_macro_enum_v<T>)
                {
                    auto copy = val;
                    changed |= ImGuidk::EnumCombo("", &copy);
                    if (changed)
                    {
                        StoreOriginalValues(display.curr_prop_path);
                        val = copy;
                        changed_and_deactivated = true;
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

                    if (ImGui::Combo("##variant", &new_ind, combo_items.data(), static_cast<int>(sz)))
                    {
                        val = variant_construct<T>(new_ind);
                        changed = true;
                        changed_and_deactivated = true;
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
                    bool dyn_handled = false;

                    if constexpr (std::is_same_v<T, reflect::dynamic>)
                    {
                        if (val.type.is_enum_type())
                        {
                            auto enum_val = val.to_enum_value();
                            if (ImGui::BeginCombo("", enum_val.name().data()))
                            {
                                for (auto [k, v] : enum_val.enum_type)
                                {
                                    if (ImGui::Selectable(string(k).c_str(), v == enum_val.value()))
                                    {
                                        StoreOriginalValues(display.curr_prop_path);
                                        val = enum_val.try_assign(v).value();
                                        changed = true;
                                        changed_and_deactivated = true;
                                    }
                                }
                                ImGui::EndCombo();
                            }
                            dyn_handled = true;
                        }
                    }

                    if (!dyn_handled)
                    {
                        recurse = true;
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
                    }
                }
            }

            display.ItemEnd();

            if (ImGui::IsItemActive() && ImGui::GetCurrentContext()->ActiveIdIsJustActivated)
                StoreOriginalValues(display.curr_prop_path);
            else if (ImGui::IsItemDeactivatedAfterEdit())
                changed_and_deactivated = true;
            if (changed_and_deactivated)
                ExecuteModify(display.curr_prop_path, reflect::dynamic(val).copy());

            // assign changed value to all other selected objects
            if (changed)
            {
                const auto& sel_obj = Core::GetSystem<IDE>().GetSelectedObjects().game_objects;

                // execute for similar components, i == 0 is the current displaying component
                for (size_t i = 1; i < sel_obj.size(); ++i)
                {
                    auto obj = sel_obj[i];

                    auto components = obj->GetComponents();
                    auto nth = _curr_component_nth;
                    for (auto c : components)
                    {
                        if (c.type == _curr_component.type && nth-- == 0)
                        {
                            assign_property_path(*c, display.curr_prop_path, val);
                            break;
                        }
                    }
                }
            }

            outer_changed |= changed;
            display.GroupEnd();

            indent_stack.push_back(indent);
            if (indent)
                ImGui::Indent();

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

        if (dyn.is<mono::Behavior>())
        {
            _curr_property_stack.push_back("script_data");
            dyn.get<mono::Behavior>().GetObject().Visit(generic_visitor, _debug_mode);
        }
        else
            dyn.visit(generic_visitor);

        std::swap(prop_stack_copy, _curr_property_stack);
        for (auto i : indent_stack)
        {
            if (i)
                ImGui::Unindent();
        }

        return outer_changed;
    }



    void IGE_InspectorWindow::DisplayStack::ItemBegin(bool align)
    {
        if (align)
        {
            ImGui::SameLine();
            ImGui::SetCursorPosX(ImGui::GetWindowContentRegionWidth() * (1.0f - item_width_ratio));
        }
        ImGui::PushID(curr_prop_path.c_str());
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
                (*self._curr_component).type.name(), curr_prop_path, self._curr_component_nth);

        ImGui::BeginGroup();
    }

    void IGE_InspectorWindow::DisplayStack::GroupEnd()
    {
        ImGui::EndGroup();

        if (has_override && ImGui::BeginPopupContextItem("__context"))
        {
            if (ImGui::MenuItem("Apply Property"))
            {
                PropertyOverride ov{ string((*self._curr_component).type.name()), curr_prop_path, self._curr_component_nth };
                PrefabUtility::ApplyPropertyOverride(self._prefab_inst->GetGameObject(), ov);
            }
            if (ImGui::MenuItem("Revert Property"))
            {
                PropertyOverride ov{ string((*self._curr_component).type.name()), curr_prop_path, self._curr_component_nth };
                PrefabUtility::RevertPropertyOverride(self._prefab_inst->GetGameObject(), ov);
            }
            ImGui::EndPopup();
        }

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
