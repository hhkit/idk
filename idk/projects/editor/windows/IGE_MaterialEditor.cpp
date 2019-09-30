#include "pch.h"

#include "IGE_MaterialEditor.h"
#include <editor/IDE.h>
#include <editor/windows/IGE_ProjectWindow.h>
#include <gfx/ShaderGraph.h>
#include <gfx/ShaderGraph_helpers.h>
#include <editor/widgets/InputResource.h>
#include <editor/widgets/EnumCombo.h>
#include <regex>
#include <filesystem>

namespace fs = std::filesystem;

namespace idk
{
    using namespace idk::shadergraph;

    static array<unsigned, ValueType::count + 1> type_colors
    { // from DB32 palette
        0,
        0xff826030,
        0xffe16e5b,
        0xffff9b63,
        0xffe4fc5f,
        0xff6e9437,
        0xff30be6a,
        0xff50e599,
        0xffba7bd7
    };



    static const char* slot_suffix(ValueType type)
    {
        switch (type)
        {
        case ValueType::FLOAT:      return "(1)";
        case ValueType::VEC2:       return "(2)";
        case ValueType::VEC3:       return "(3)";
        case ValueType::VEC4:       return "(4)";
        case ValueType::SAMPLER2D:  return "(T)";
        default: throw;
        }
    }

    bool draw_slot(const char* title, int kind)
    {
        auto* canvas = ImNodes::GetCurrentCanvas();

        auto* storage = ImGui::GetStateStorage();
        const auto& style = ImGui::GetStyle();
        const float CIRCLE_RADIUS = 5.f * canvas->zoom;
        ImVec2 title_size = ImGui::CalcTextSize(title);

        // Pull entire slot a little bit out of the edge so that curves connect into int without visible seams
        float item_offset_x = style.ItemSpacing.x * canvas->zoom;
        if (!ImNodes::IsOutputSlotKind(kind))
            item_offset_x = -item_offset_x;
        ImGui::SetCursorScreenPos(ImGui::GetCursorScreenPos() + ImVec2{ item_offset_x, 0 });

        if (ImNodes::IsOutputSlotKind(kind))
        {
            // Align output slots to the right edge of the node.
            ImGuiID max_width_id = ImGui::GetID("output-max-title-width");
            float offset = storage->GetFloat(max_width_id, title_size.x) - title_size.x;
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offset);
        }

        if (ImNodes::BeginSlot(title, kind))
        {
            auto* draw_lists = ImGui::GetWindowDrawList();

            // Slot appearance can be altered depending on curve hovering state.
            bool is_active = ImNodes::IsSlotCurveHovered() ||
                (ImNodes::IsConnectingCompatibleSlot() /*&& !IsAlreadyConnectedWithPendingConnection(title, kind)*/);

            ImColor kindColor = type_colors[std::abs(kind)];
            ImColor text_color = ImGui::GetColorU32(ImGuiCol_Text);//is_active ? kindColor : canvas->colors[ImNodes::ColConnection];
            if (!is_active) text_color.Value.w = 0.7f;
            ImColor color = is_active ? canvas->colors[ImNodes::ColConnectionActive] : canvas->colors[ImNodes::ColConnection];

            string label = title;
            label += slot_suffix(static_cast<ValueType>(abs(kind)));

            ImGui::PushStyleColor(ImGuiCol_Text, text_color.Value);

            if (ImNodes::IsOutputSlotKind(kind))
            {
                ImGui::TextUnformatted(label.c_str());
                ImGui::SameLine();
            }

            ImRect circle_rect{
                ImGui::GetCursorScreenPos(),
                ImGui::GetCursorScreenPos() + ImVec2{CIRCLE_RADIUS * 2, CIRCLE_RADIUS * 2}
            };
            // Vertical-align circle in the middle of the line.
            float circle_offset_y = title_size.y / 2.f - CIRCLE_RADIUS;
            circle_rect.Min.y += circle_offset_y;
            circle_rect.Max.y += circle_offset_y;
            draw_lists->AddCircleFilled(circle_rect.GetCenter(), CIRCLE_RADIUS, color);

            ImGui::ItemSize(circle_rect.GetSize());
            ImGui::ItemAdd(circle_rect, ImGui::GetID(title));

            if (ImNodes::IsInputSlotKind(kind))
            {
                ImGui::SameLine();
                ImGui::TextUnformatted(label.c_str());
            }

            ImGui::PopStyleColor();
            ImNodes::EndSlot();

            // A dirty trick to place output slot circle on the border.
            ImGui::GetCurrentWindow()->DC.CursorMaxPos.x -= item_offset_x;
            return true;
        }
        return false;
    }



    static bool draw_value_draw_vec(const char* id, vec2 pos, int n, float* f)
    {
        auto canvas = ImNodes::GetCurrentCanvas();
        auto& style = ImGui::GetStyle();

        const float itemw = (40.0f + style.ItemSpacing.x) * n;
        pos.x -= itemw + 4.0f;

        auto screen_pos = ImGui::GetWindowPos() + pos * canvas->zoom + canvas->offset;

        ImGui::SetCursorScreenPos(screen_pos + ImVec2{ 1.0f, 1.0f });

        ImGui::BeginGroup();

        ImRect rect = { screen_pos, screen_pos + ImVec2{ itemw * canvas->zoom, ImGui::GetTextLineHeight() + 2.0f } };

        ImGui::GetWindowDrawList()->AddRectFilled(rect.Min, rect.Max,
                                                  canvas->colors[ImNodes::ColNodeActiveBg], style.FrameRounding);
        ImGui::GetWindowDrawList()->AddRect(rect.Min, rect.Max,
                                            canvas->colors[ImNodes::ColNodeActiveBg], style.FrameRounding);

        ImGui::PushItemWidth(itemw * canvas->zoom);
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 1.0f, 0.0f });

        bool ret = false;

        switch (n)
        {
        case 1: ret = ImGui::DragFloat (("##" + std::string(id)).c_str(), f, 0.01f); break;
        case 2: ret = ImGui::DragFloat2(("##" + std::string(id)).c_str(), f, 0.01f); break;
        case 3: ret = ImGui::DragFloat3(("##" + std::string(id)).c_str(), f, 0.01f); break;
        case 4: ret = ImGui::DragFloat4(("##" + std::string(id)).c_str(), f, 0.01f); break;
        default: throw;
        }

        ImGui::PopStyleVar();
        ImGui::PopItemWidth();

        ImGui::EndGroup();

        return ret;
    }

    void IGE_MaterialEditor::drawValue(Node& node, int input_slot_index)
    {
        auto pos = node.position;
        pos.y += (ImGui::GetTextLineHeightWithSpacing() + ImGui::GetStyle().ItemSpacing.y) / _canvas.zoom * (input_slot_index + 1);

        auto& slot = node.input_slots[input_slot_index];
        if (slot.value.empty())
            return;

        auto id = string(node.guid) + std::to_string(input_slot_index);

        switch (slot.type)
        {

        case ValueType::FLOAT:
        {
            float f = std::stof(slot.value);
            if (draw_value_draw_vec(id.c_str(), pos, 1, &f))
                slot.value = std::to_string(f);
            break;
        }
        case ValueType::VEC2:
        {
            vec2 v = helpers::parse_vec2(slot.value);
            if (draw_value_draw_vec(id.c_str(), pos, 2, v.values))
                slot.value = helpers::serialize_value(v);
            break;
        }
        case ValueType::VEC3:
        {
            vec3 v = helpers::parse_vec3(slot.value);
            if (draw_value_draw_vec(id.c_str(), pos, 3, v.values))
                slot.value = helpers::serialize_value(v);
            break;
        }
        case ValueType::VEC4:
        {
            vec4 v = helpers::parse_vec4(slot.value);
            if (draw_value_draw_vec(id.c_str(), pos, 4, v.values))
                slot.value = helpers::serialize_value(v);
            break;
        }

        default:
            break;
        }
    }

    void IGE_MaterialEditor::addDefaultSlotValue(const Guid& guid, int slot_in)
    {
        auto& node_in = _graph->nodes[guid];
        auto in_type = node_in.input_slots[slot_in].type;
        node_in.input_slots[slot_in].value = helpers::default_value(in_type);
    }



    void IGE_MaterialEditor::drawNode(Node& node)
    {
        bool is_master_node = node.guid == _graph->master_node;
        bool is_param_node = node.name[0] == '$';

        _canvas.colors[ImNodes::ColNodeBorder] = node.selected ? _canvas.colors[ImNodes::ColConnectionActive] : _canvas.colors[ImNodes::ColNodeBg];

        if (ImNodes::BeginNode(&node, reinterpret_cast<ImVec2*>(&node.position), &node.selected))
        {
            const auto slash_pos = node.name.find_last_of('\\');
            
            string title = node.name.c_str() + (slash_pos == std::string::npos ? 0 : slash_pos + 1);
            if (is_master_node)
                title += " (Master)";
            if (is_param_node)
                title = "Parameter";

            auto title_size = ImGui::CalcTextSize(title.c_str());
            float input_names_width = 0;
            float output_names_width = 0;

            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetStyle().ItemSpacing.x * 0.5f);
            ImGui::Text(title.c_str());

            ImGui::BeginGroup();

            if (is_param_node)
            {
                auto& param = _graph->parameters[std::stoi(node.name.data() + 1)];
                const auto& slot_name = param.name;
                auto slot_w = ImGui::CalcTextSize(slot_name.c_str()).x + ImGui::CalcTextSize(slot_suffix(param.type)).x;
                title_size.x = std::max(title_size.x, slot_w);
                ImGui::GetStateStorage()->SetFloat(ImGui::GetID("output-max-title-width"), title_size.x);

                draw_slot(slot_name.c_str(), param.type);
            }
            else
            {
                auto& tpl = NodeTemplate::GetTable().at(node.name);

                int i = 0;
                size_t num_slots = node.input_slots.size() + node.output_slots.size();

                // calc width of node
                for (auto& slot_name : tpl.names)
                {
                    if (i < node.input_slots.size())
                    {
                        const auto suffix_w = ImGui::CalcTextSize(slot_suffix(node.input_slots[i].type)).x;
                        input_names_width = std::max(input_names_width, ImGui::CalcTextSize(slot_name.c_str()).x + suffix_w);
                    }
                    else if (i < num_slots)
                    {
                        const auto suffix_w = ImGui::CalcTextSize(slot_suffix(node.output_slots[i - node.input_slots.size()].type)).x;
                        output_names_width = std::max(output_names_width, ImGui::CalcTextSize(slot_name.c_str()).x + suffix_w);
                    }
                    else
                        break;
                    ++i;
                }
                title_size.x = std::max(title_size.x, input_names_width + output_names_width);
                ImGui::GetStateStorage()->SetFloat(ImGui::GetID("output-max-title-width"), title_size.x);

                i = 0;
                float cursor_y = ImGui::GetCursorPosY();
                string control_values = node.control_values;
                string final_control_values;

                for (auto& slot_name : tpl.names)
                {
                    if (i == node.input_slots.size())
                        ImGui::SetCursorPosY(cursor_y);
                    ImGui::Spacing();

                    int kind = 0;
                    if (i < node.input_slots.size())
                    {
                        Node* node_out;
                        const char* title_out;
                        int kind_out;

                        kind = -node.input_slots[i].type;

                        if (ImNodes::GetPendingConnection(reinterpret_cast<void**>(&node_out), &title_out, &kind_out))
                        {
                            for (auto& sig : tpl.signatures)
                            {
                                if (sig.ins[i] == kind_out)
                                {
                                    kind = -kind_out;
                                    break;
                                }
                            }
                        }

                        auto cursorpos = ImGui::GetCursorPos();
                        drawValue(node, i); // draw value control when disconnected
                        ImGui::SetCursorPos(cursorpos);
                        draw_slot(slot_name.c_str(), kind);
                    }
                    else if (i < num_slots)
                    {
                        kind = node.output_slots[i - node.input_slots.size()].type;
                        draw_slot(slot_name.c_str(), kind);
                    }
                    else // custom controls
                    {
                        assert(slot_name[0] == '$'); // did we fuck up the names in the .node?

                        string next_value = "";
                        if (control_values.size())
                        {
                            auto pos = control_values.find('|');
                            next_value = control_values.substr(0, pos);
                            if (pos != string::npos)
                                control_values.erase(0, pos + 1);
                            else
                                control_values.clear();
                        }

                        if (slot_name == "$Color")
                        {
                            auto w = ImGui::GetStyle().ItemSpacing.x * 4 * _canvas.zoom + ImGui::GetStateStorage()->GetFloat(ImGui::GetID("output-max-title-width"));

                            if (next_value.empty())
                                final_control_values += (next_value = "0,0,0,1") + '|';

                            vec4 v = helpers::parse_vec4(next_value);
                            if (ImGui::ColorButton(("##" + std::to_string(i)).c_str(), v, 0, ImVec2(w, 0)))
                            {
                                ImGui::OpenPopup("picker");
                                ImGui::GetCurrentContext()->ColorPickerRef = v;
                            }
                            if (ImGui::BeginPopup("picker"))
                            {
                                ImGuiColorEditFlags picker_flags = ImGuiColorEditFlags__DisplayMask | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_AlphaPreviewHalf;
                                ImGui::SetNextItemWidth(ImGui::GetFrameHeight() * 12.0f); // Use 256 + bar sizes?
                                if (ImGui::ColorPicker4("##picker", v.values, picker_flags, &ImGui::GetCurrentContext()->ColorPickerRef.x))
                                    final_control_values += helpers::serialize_value(v) + "|";

                                ImGui::EndPopup();
                            }
                        }
                    }

                    ++i;
                }

                if (final_control_values.size())
                    node.control_values = final_control_values;
            }

            ImGui::EndGroup();

            ImNodes::EndNode();
        }

        // if context menu is already open, iswindowhovered will return false
        auto can_open = ImGui::IsPopupOpen(ImGui::GetCurrentWindow()->DC.LastItemId) ||
            ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem);
        can_open = can_open && !ImGui::IsMouseDragPastThreshold(1);
        auto fntscl = ImGui::GetCurrentWindow()->FontWindowScale;
        ImGui::SetWindowFontScale(1.0f);
        if (can_open && ImGui::BeginPopupContextItem())
        {
            if (ImGui::MenuItem("Disconnect"))
                disconnectNode(node);
            ImGui::Separator();
            if (ImGui::MenuItem("Cut", NULL, false, !is_master_node))
            {

            }
            if (ImGui::MenuItem("Copy", NULL, false, !is_master_node))
            {

            }
            if (ImGui::MenuItem("Delete", NULL, false, !is_master_node))
            {
                removeNode(node);
            }
            if (ImGui::MenuItem("Duplicate", NULL, false, !is_master_node))
            {

            }
            ImGui::EndPopup();
        }
        ImGui::SetWindowFontScale(fntscl);
    }

    Node& IGE_MaterialEditor::addNode(const string& name, vec2 pos)
    {
        Node node;
        node.name = name;
        node.guid = Guid::Make();
        node.position = pos;

        auto sig = NodeTemplate::GetTable().at(name).signatures[0];
        for (auto in : sig.ins)
            node.input_slots.push_back({ in, helpers::default_value(in) });
        for (auto out : sig.outs)
            node.output_slots.push_back({ out });

        return _graph->nodes.emplace(node.guid, node).first->second;
    }

    void IGE_MaterialEditor::removeNode(const Node& node)
    {
        disconnectNode(node);
        _nodes_to_delete.push_back(node.guid);
    }

    void IGE_MaterialEditor::disconnectNode(const Node& node)
    {
        auto& g = *_graph;
        auto iter = std::remove_if(g.links.begin(), g.links.end(),
            [guid = node.guid](const Link& link) { return link.node_in == guid || link.node_out == guid; });
        for (auto jter = iter; jter != g.links.end(); ++jter)
            addDefaultSlotValue(jter->node_in, jter->slot_in);
        g.links.erase(iter, g.links.end());
    }



    void IGE_MaterialEditor::addParamNode(int param_index, vec2 pos)
    {
        auto& param = _graph->parameters[param_index];

        Node node;
        node.name = "$" + std::to_string(param_index);
        node.guid = Guid::Make();
        node.position = pos;
        node.output_slots.push_back(Slot{ param.type });

        _graph->nodes.emplace(node.guid, node);
    }

    void IGE_MaterialEditor::removeParam(int param_index)
    {
        // since all params are referenced by indices,
        // we have to shift all param nodes value that reference greater indices
        // as when you erase a param, all params after that are shifted back by 1.

        vector<Guid> to_del;
        for (auto& [guid, node] : _graph->nodes)
        {
            if (node.name[0] != '$')
                continue;

            auto index = std::stoi(node.name.data() + 1);
            if (index > param_index)
                node.name = '$' + std::to_string(index - 1);
            else if (index == param_index)
                to_del.push_back(guid);
        }
        for (const auto& guid : to_del)
            removeNode(_graph->nodes[guid]);

        _graph->parameters.erase(_graph->parameters.begin() + param_index);
    }



    struct node_item { vector<node_item> items; string name; };

    static node_item* context_menu_node_item(node_item& item, std::vector<node_item*>& stack)
    {
        if (item.items.size())
        {
            auto folder_name = item.name;
            if (folder_name == "master") // skip master nodes
                return nullptr;
            if (stack.empty() || ImGui::TreeNodeEx(folder_name.c_str(), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanAllAvailWidth))
            {
                stack.push_back(&item);
                for (auto& inner_item : item.items)
                {
                    auto* res = context_menu_node_item(inner_item, stack);
                    if (res)
                    {
                        if (item.name.size())
                            ImGui::TreePop();
                        return res;
                    }
                }
                stack.pop_back();

                if (stack.size())
                    ImGui::TreePop();
            }
        }
        else
        {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{ 0.7f, 0.7f, 0.7f, 1.0f });
			ImGui::Unindent();
            ImGui::TreeNodeEx(item.name.c_str(), ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanAllAvailWidth);
			ImGui::Indent();
			if (ImGui::IsItemClicked())
			{
				ImGui::PopStyleColor();
				return &item;
			}
            ImGui::PopStyleColor();
        }

        return nullptr;
    }

    static node_item init_templates_hierarchy()
    {
        node_item root;
		auto get_uppercase = [](const string& str)
		{
			auto ret = str;
			for (char& c : ret)
				c = static_cast<char>(toupper(c));
			return ret;
		};

        for (auto& tpl : NodeTemplate::GetTable())
        {
            node_item* curr = &root;
            fs::path path = tpl.first;

            for (auto& part : path)
            {
                auto iter = std::find_if(curr->items.begin(), curr->items.end(), [&](node_item& item) { return item.name == part; });
                if (iter != curr->items.end())
                    curr = &*iter;
                else
                {
                    curr->items.push_back({ {}, part.string() });
                    curr = &curr->items.back();
                }
            }
        }
        std::vector<node_item*> stack{ &root };
        while (stack.size())
        {
            auto* item = stack.back();
            stack.pop_back();
            std::sort(item->items.begin(), item->items.end(),
				[get_uppercase](node_item& a, node_item& b)
				{ 
					return get_uppercase(a.name) < get_uppercase(b.name);
				});
            for (auto& inner_item : item->items)
            {
                stack.push_back(&inner_item);
            }
        }

        return root;
    }

    static string draw_nodes_context_menu()
    {
        static node_item templates_hierarchy = init_templates_hierarchy();
        std::vector<node_item*> stack;

        auto* item = context_menu_node_item(templates_hierarchy, stack);
        
        if (item)
        {
            fs::path path;
            // build path
            for (auto iter = stack.begin() + 1; iter != stack.end(); ++iter)
                path /= (*iter)->name;
            path /= item->name;

            return path.string();
        }

        return "";
    }



    void IGE_MaterialEditor::OpenGraph(const RscHandle<shadergraph::Graph>& handle)
    {
		_graph = handle;

        ImGui::SetWindowFocus(window_name);
        is_open = true;
    }



    IGE_MaterialEditor::IGE_MaterialEditor()
        : IGE_IWindow("Material Editor", false, ImVec2{ 600,300 }, ImVec2{ 0,0 })
    {
        window_flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_MenuBar;
    }

    void IGE_MaterialEditor::Initialize()
    {
        _canvas.colors[ImNodes::ColNodeBg] = ImGui::GetColorU32(ImGuiCol_Border);
        _canvas.colors[ImNodes::ColNodeActiveBg] = ImGui::GetColorU32(ImGuiCol_Border);
        _canvas.style.curve_thickness = 2.5f;

        Core::GetSystem<IDE>().FindWindow<IGE_ProjectWindow>()->OnAssetDoubleClicked.Listen([&](GenericResourceHandle handle)
        {
            if (handle.resource_id() == BaseResourceID<Graph>)
                OpenGraph(handle.AsHandle<Graph>());
        });
    }

    void IGE_MaterialEditor::BeginWindow()
    {
    }

    void IGE_MaterialEditor::Update()
    {
        if (!is_open || ImGui::IsWindowCollapsed())
        {
            ImNodes::BeginCanvas(&_canvas);
            ImNodes::EndCanvas();
            return;
        }

        if (!_graph)
        {
            ImNodes::BeginCanvas(&_canvas);
            ImNodes::EndCanvas();
            //graph = Core::GetResourceManager().Create<Graph>();
            //addNode("master\\PBR", { 500.0f, 200.0f });
            //g.master_node = g.nodes.begin()->first;
            //Core::GetSystem<SaveableResourceManager>().Save(graph);
            return;
        }

        auto& g = *_graph;


        if (ImGui::BeginMenuBar())
        {
            if (ImGui::Button("Compile & Save"))
            {
                g.Compile();
                Core::GetResourceManager().Save(_graph);
            }
            ImGui::EndMenuBar();
        }


        auto window_pos = ImGui::GetWindowPos();

        ImGui::SetWindowFontScale(1.0f);
        if (!ImGui::IsMouseDragPastThreshold(1) && ImGui::IsMouseReleased(1) && !ImGui::IsAnyItemHovered() && ImGui::IsWindowHovered())
            ImGui::OpenPopup("nodes_context_menu");
        if (ImGui::IsPopupOpen("nodes_context_menu"))
            ImGui::SetNextWindowSizeConstraints(ImVec2{ 200, 320 }, ImVec2{ 200, 320 });
        if (ImGui::BeginPopup("nodes_context_menu"))
        {
            auto str = draw_nodes_context_menu();
            if (str.size())
            {
                ImGui::CloseCurrentPopup();
                auto pos = (ImGui::GetWindowPos() - window_pos - _canvas.offset) / _canvas.zoom;
                addNode(str, pos);
                // pos = windowpos + nodepos * zoom + offset
                // nodepos = (screenpos - offset - windowpos) / zoom
            }
            ImGui::EndPopup();
        }

        for (auto& guid : _nodes_to_delete)
            g.nodes.erase(guid);

        ImNodes::BeginCanvas(&_canvas);

        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.0f);

        for (auto& node : g.nodes)
            drawNode(node.second);

        for(auto& guid : _nodes_to_delete)
            g.nodes.erase(guid);

        ImGui::PopStyleVar();

        auto connection_col_active = _canvas.colors[ImNodes::ColConnectionActive];
        {
            Node* node;
            const char* title;
            int kind;

            if (ImNodes::GetPendingConnection(r_cast<void**>(&node), &title, &kind))
            {
                _canvas.colors[ImNodes::ColConnectionActive] = type_colors[std::abs(kind)];
            }
        }

        // handle new connection
        {
            Node* node_out;
            Node* node_in;
            const char* title_out;
            const char* title_in;
            if (ImNodes::GetNewConnection(r_cast<void**>(&node_in), &title_in, r_cast<void**>(&node_out), &title_out))
            {
                int slot_in = (int)NodeTemplate::GetTable().at(node_in->name).GetSlotIndex(title_in);

                bool out_is_param = node_out->name[0] == '$';
                int slot_out = out_is_param ? 0 : (int)NodeTemplate::GetTable().at(node_out->name).GetSlotIndex(title_out);

                // check if any link inputs into input slot (cannot have multiple)
                for (auto iter = g.links.begin(); iter != g.links.end(); ++iter)
                {
                    if (iter->node_in == node_in->guid && iter->slot_in == slot_in)
                    {
                        g.links.erase(iter);
                        break;
                    }
                }

                g.links.push_back({ node_out->guid, node_in->guid, slot_out, slot_in });

                // clear any unconnected value
                node_in->input_slots[slot_in].value.clear();
                // set new type
                node_in->input_slots[slot_in].type = node_out->output_slots[slot_out - node_out->input_slots.size()].type;

                // resolve new node_in output type
                if (!out_is_param)
                {
                    auto& tpl = NodeTemplate::GetTable().at(node_in->name);
                    for (auto& sig : tpl.signatures)
                    {
                        bool match = true;
                        for (size_t i = 0; i < node_in->input_slots.size(); ++i)
                        {
                            if (node_in->input_slots[i].type != sig.ins[i])
                                match = false;
                        }
                        if (match)
                        {
                            for (size_t i = 0; i < node_in->output_slots.size(); ++i)
                                node_in->output_slots[i].type = sig.outs[i];
                            break;
                        }
                    }
                }
            }
        }

        auto link_to_delete = g.links.end();
        for (auto iter = g.links.begin(); iter != g.links.end(); ++iter)
        {
            auto& link = *iter;

            auto& node_out = g.nodes[link.node_out];
            auto& node_in = g.nodes[link.node_in];
            auto col = _canvas.colors[ImNodes::ColConnection];
            _canvas.colors[ImNodes::ColConnection] = type_colors[std::abs(node_in.input_slots[link.slot_in].type)];

            const auto& slot_out = node_out.name[0] == '$' ?
                g.parameters[std::stoi(node_out.name.data() + 1)].name :
                NodeTemplate::GetTable().at(node_out.name).names[link.slot_out];

            if (!ImNodes::Connection(&node_in, NodeTemplate::GetTable().at(node_in.name).names[link.slot_in].c_str(),
                                     &node_out, slot_out.c_str()))
            {
                link_to_delete = iter;
            }
            _canvas.colors[ImNodes::ColConnection] = col;
        }
        if (link_to_delete != g.links.end())
        {
            addDefaultSlotValue(link_to_delete->node_in, link_to_delete->slot_in);
            g.links.erase(link_to_delete);
        }

        ImNodes::EndCanvas();
        _canvas.colors[ImNodes::ColConnectionActive] = connection_col_active;




        ImGui::SetCursorPos(ImGui::GetWindowContentRegionMin());
        ImGui::Dummy(ImVec2(ImGui::GetContentRegionAvail()));
        if (ImGui::BeginDragDropTarget())
        {
            if (const auto* payload = ImGui::AcceptDragDropPayload(DragDrop::PARAMETER, ImGuiDragDropFlags_AcceptNoDrawDefaultRect))
            {
                addParamNode(*reinterpret_cast<int*>(payload->Data), (ImGui::GetMousePos() - ImGui::GetWindowPos() - _canvas.offset) / _canvas.zoom);
            }
            ImGui::EndDragDropTarget();
        }



        show_params_window();
    }

    void IGE_MaterialEditor::show_params_window()
    {
        ImGui::SetCursorPos(ImGui::GetWindowContentRegionMin());

        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImGui::GetStyleColorVec4(ImGuiCol_WindowBg));
        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 2.0f);

        if (ImGui::BeginChild("Material_Editor_miniwin", ImVec2(200, 300), true, ImGuiWindowFlags_NoMove))
        {
            auto graph_name = Core::GetResourceManager().GetPath(_graph);
            ImGui::Text(graph_name->data());

            auto meta = _graph->GetMeta();
            bool changed = false;
            changed = changed || ImGuidk::EnumCombo("Domain", &meta.domain);
            changed = changed || ImGuidk::EnumCombo("Blend", &meta.blend);
            changed = changed || ImGuidk::EnumCombo("Model", &meta.model);
            if (changed)
            {
                //_graph->SetMeta(meta);

                //auto master_node = _graph->nodes[_graph->master_node];
                //auto new_master_node_name = master_node.name;
                //if (meta.domain == MaterialDomain::Surface && meta.blend == BlendMode::Opaque && meta.model == ShadingModel::DefaultLit)
                //    new_master_node_name = "master\\PBR";
                //if (meta.domain == MaterialDomain::Surface && meta.blend == BlendMode::Opaque && meta.model == ShadingModel::Unlit)
                //    new_master_node_name = "master\\Unlit";

                //if (master_node.name != new_master_node_name)
                //{
                //    auto pos = master_node.position;
                //    removeNode(master_node);
                //    _graph->master_node = addNode(new_master_node_name, pos).guid;
                //}
            }

            if (ImGui::Button("Add Parameter"))
                ImGui::OpenPopup("addparams");
            if (ImGui::BeginPopup("addparams"))
            {
                if (ImGui::MenuItem("Float"))     _graph->parameters.emplace_back(Parameter{ "NewParameter", ValueType::FLOAT,     "0" });
                if (ImGui::MenuItem("Vec2"))      _graph->parameters.emplace_back(Parameter{ "NewParameter", ValueType::VEC2,      "0,0" });
                if (ImGui::MenuItem("Vec3"))      _graph->parameters.emplace_back(Parameter{ "NewParameter", ValueType::VEC3,      "0,0,0" });
                if (ImGui::MenuItem("Vec4"))      _graph->parameters.emplace_back(Parameter{ "NewParameter", ValueType::VEC4,      "0,0,0,0" });
                if (ImGui::MenuItem("Texture"))   _graph->parameters.emplace_back(Parameter{ "NewParameter", ValueType::SAMPLER2D, string(Guid()) });
                ImGui::EndPopup();
            }

            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(1.0f, 1.0f));

            static char buf[32];
            int i = -1;
            int to_del = -1;
            for (auto& param : _graph->parameters)
            {
                ++i;

                auto* draw_list = ImGui::GetWindowDrawList();
                draw_list->ChannelsSplit(2);
                draw_list->ChannelsSetCurrent(1);

                ImGui::PushID(i);
                ImGui::BeginGroup();
                ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(2.0f, 2.0f));
                ImGui::BeginGroup();

                strcpy_s(buf, param.name.c_str());
                string label{ param.type.to_string() };
                if (param.type == ValueType::SAMPLER2D)
                    label = "TEXTURE";
                if (ImGui::InputText(label.c_str(), buf, 32))
                {
                    param.name = buf;
                }

                switch (param.type)
                {
                case ValueType::FLOAT:
                {
                    float f = helpers::parse_float(param.default_value);
                    if (ImGui::DragFloat("Default", &f, 0.01f))
                        param.default_value = helpers::serialize_value(f);
                    break;
                }
                case ValueType::VEC2:
                {
                    vec2 v = helpers::parse_vec2(param.default_value);
                    if (ImGui::DragFloat3("Default", v.values, 0.01f))
                        param.default_value = helpers::serialize_value(v);
                    break;
                }
                case ValueType::VEC3:
                {
                    vec3 v = helpers::parse_vec3(param.default_value);
                    if (ImGui::DragFloat3("Default", v.values, 0.01f))
                        param.default_value = helpers::serialize_value(v);
                    break;
                }
                case ValueType::VEC4:
                {
                    vec4 v = helpers::parse_vec4(param.default_value);
                    if (ImGui::DragFloat4("Default", v.values, 0.01f))
                        param.default_value = helpers::serialize_value(v);
                    break;
                }
                case ValueType::SAMPLER2D:
                {
                    RscHandle<Texture> tex = helpers::parse_sampler2d(param.default_value);
                    if (ImGuidk::InputResource("Default", &tex))
                        param.default_value = helpers::serialize_value(tex);
                    break;
                }
                default:
                    break;
                }

                ImGui::EndGroup();
                ImGui::EndGroup();

                ImGui::PopID();

                if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
                {
                    ImGui::Text(param.name.c_str());
                    ImGui::SetDragDropPayload(DragDrop::PARAMETER, &i, sizeof(i));
                    ImGui::EndDragDropSource();
                }

                string id = "context_" + i;
                if (ImGui::BeginPopupContextItem(id.c_str()))
                {
                    if (ImGui::MenuItem("Delete"))
                    {
                        to_del = i;
                    }
                    ImGui::EndPopup();
                }

                draw_list->ChannelsSetCurrent(0);
                auto min = ImGui::GetItemRectMin();
                auto max = ImVec2(min.x + ImGui::GetWindowContentRegionWidth(), ImGui::GetItemRectMax().y) + ImVec2(2.0f, 2.0f);
                draw_list->AddRect(min, max, ImGui::GetColorU32(ImGuiCol_Border), 2.0f);

                draw_list->ChannelsMerge();
            }

            if (to_del >= 0)
                removeParam(to_del);

            ImGui::PopStyleVar();
        }
        ImGui::EndChild();

        ImGui::PopStyleVar();
        ImGui::PopStyleColor();
    }

}