#include "pch.h"

#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif

#include "IGE_MaterialEditor.h"
#include <gfx/ShaderGraph.h>
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
        0xffba7b37
    };

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

        if (ImNodes::BeginSlot(title, kind))
        {
            auto* draw_lists = ImGui::GetWindowDrawList();

            // Slot appearance can be altered depending on curve hovering state.
            bool is_active = ImNodes::IsSlotCurveHovered() ||
                (ImNodes::IsConnectingCompatibleSlot() /*&& !IsAlreadyConnectedWithPendingConnection(title, kind)*/);

            ImColor kindColor = type_colors[std::abs(kind)];
            ImColor color = is_active ? kindColor : canvas->colors[ImNodes::ColConnection];

            ImGui::PushStyleColor(ImGuiCol_Text, color.Value);

            if (ImNodes::IsOutputSlotKind(kind))
            {
                // Align output slots to the right edge of the node.
                ImGuiID max_width_id = ImGui::GetID("output-max-title-width");
                float output_max_title_width = ImMax(storage->GetFloat(max_width_id, title_size.x), title_size.x);
                storage->SetFloat(max_width_id, output_max_title_width);
                float offset = (output_max_title_width + style.ItemSpacing.x) - title_size.x;
                ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offset);

                ImGui::TextUnformatted(title);
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
                ImGui::TextUnformatted(title);
            }

            ImGui::PopStyleColor();
            ImNodes::EndSlot();

            // A dirty trick to place output slot circle on the border.
            ImGui::GetCurrentWindow()->DC.CursorMaxPos.x -= item_offset_x;
            return true;
        }
        return false;
    }

    void IGE_MaterialEditor::drawNode(Node& node)
    {
        bool is_master_node = node.guid == graph->master_node;
        canvas.colors[ImNodes::ColNodeBorder] = node.selected ? canvas.colors[ImNodes::ColConnectionActive] : canvas.colors[ImNodes::ColNodeBg];

        if (ImNodes::BeginNode(&node, r_cast<ImVec2*>(&node.position), &node.selected))
        {
            auto& tpl = NodeTemplate::GetTable().at(node.name);

            auto slash_pos = node.name.find_last_of('\\');
            string title = node.name.c_str() + (slash_pos == std::string::npos ? 0 : slash_pos + 1);
			if (is_master_node)
				title += " (Master)";
            auto title_size = ImGui::CalcTextSize(title.c_str());
            float input_names_width = 0;
            float output_names_width = 0;

            int i = 0;
            for (auto slot_name : tpl.names)
            {
                if (i < node.input_slots.size())
                    input_names_width = std::max(input_names_width, ImGui::CalcTextSize(slot_name.c_str()).x);
                else
                    output_names_width = std::max(output_names_width, ImGui::CalcTextSize(slot_name.c_str()).x);
                ++i;
            }

            //float item_offset_x = ImGui::GetStyle().ItemSpacing.x * ImNodes::GetCurrentCanvas()->zoom;
            title_size.x = std::max(title_size.x, input_names_width + output_names_width + ImGui::GetStyle().ItemSpacing.x * 2);

            ImGui::GetStateStorage()->SetFloat(ImGui::GetID("output-max-title-width"), title_size.x);
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetStyle().ItemSpacing.x * 0.5f);
            ImGui::Text(title.c_str());

            ImGui::BeginGroup();

            i = 0;
            float cursor_y = ImGui::GetCursorPosY();
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
                }
                else
                {
                    kind = node.output_slots[i - node.input_slots.size()].type;
                }

                draw_slot(slot_name.c_str(), kind);
                ++i;
            }

            ImGui::EndGroup();

            ImNodes::EndNode();
        }

        if (ImGui::BeginPopupContextItem())
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
    }

    static bool DrawValue_DrawVec(const char* id, vec2 pos, int n, float* f)
    {
        auto canvas = ImNodes::GetCurrentCanvas();
        auto& style = ImGui::GetStyle();

        const float itemw = (40.0f + style.ItemSpacing.x) * n;
        pos.x -= itemw + 4.0f;

        auto screen_pos = ImGui::GetWindowPos() + r_cast<ImVec2&>(pos) * canvas->zoom + canvas->offset;

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
        case 1: ret = ImGui::DragFloat(("##" + std::string(id)).c_str(), f, 0.1f);	break;
        case 2: ret = ImGui::DragFloat2(("##" + std::string(id)).c_str(), f, 0.1f);	break;
        case 3: ret = ImGui::DragFloat3(("##" + std::string(id)).c_str(), f, 0.1f);	break;
        case 4: ret = ImGui::DragFloat4(("##" + std::string(id)).c_str(), f, 0.1f);	break;
        default: throw;
        }

        ImGui::PopStyleVar();
        ImGui::PopItemWidth();

        ImGui::EndGroup();

        return ret;
    }

    void IGE_MaterialEditor::drawValue(Value& value)
    {
        auto& node = graph->nodes[value.node];
        auto pos = node.position;
        pos.y += (ImGui::GetTextLineHeightWithSpacing() + ImGui::GetStyle().ItemSpacing.y) / canvas.zoom * (value.slot + 1);

        switch (value.type)
        {

        case ValueType::FLOAT: {
            float f = std::stof(value.value);
            if (DrawValue_DrawVec((std::string(value.node) + std::to_string(value.slot)).c_str(), pos, 1, &f))
            {
                value.value = std::to_string(f);
            }
        } break;

        case ValueType::VEC2: { // todo: use serializer

            float f[2]{ 0, 0 };
            std::smatch matches;
            if (std::regex_match(value.value, matches, std::regex("([\\d\\.\\-]+),([\\d\\.\\-]+)")))
            {
                f[0] = std::stof(matches[1]);
                f[1] = std::stof(matches[2]);
            }

            if (DrawValue_DrawVec((std::string(value.node) + std::to_string(value.slot)).c_str(), pos, 2, f))
            {
                value.value = std::to_string(f[0]) + ',' + std::to_string(f[1]);
            }
        } break;

        case ValueType::VEC3: { // todo: use serializer

            float f[3]{ 0, 0, 0 };
            std::smatch matches;
            if (std::regex_match(value.value, matches, std::regex("([\\d\\.\\-]+),([\\d\\.\\-]+),([\\d\\.\\-]+)")))
            {
                f[0] = std::stof(matches[1]);
                f[1] = std::stof(matches[2]);
                f[2] = std::stof(matches[3]);
            }

            if (DrawValue_DrawVec((std::string(value.node) + std::to_string(value.slot)).c_str(), pos, 3, f))
            {
                value.value = std::to_string(f[0]) + ',' + std::to_string(f[1]) + ',' + std::to_string(f[2]);
            }
        } break;

        default:
            break;
        }

    }

    string default_value(ValueType type)
    {
        switch (type)
        {
        case ValueType::FLOAT: return "0";
        case ValueType::VEC2: return "0,0";
        case ValueType::VEC3: return "0,0,0";
        default: throw;
        }
    }

    void IGE_MaterialEditor::addNode(const string& name, vec2 pos)
    {
        Node node;
        node.name = name;
        node.guid = Guid::Make();
        node.position = pos;

        auto sig = NodeTemplate::GetTable().at(name).signatures[0];
        for (auto in : sig.ins)
        {
            node.input_slots.push_back({ in });
            graph->values.emplace_back(Value{ in, default_value(in), node.guid, static_cast<int>(node.input_slots.size() - 1) });
        }
        for (auto out : sig.outs)
            node.output_slots.push_back({ out });

        graph->nodes[node.guid] = node;
    }

    void IGE_MaterialEditor::removeNode(const Node& node)
    {
        disconnectNode(node);
        auto& g = *graph;
        g.values.erase(std::remove_if(g.values.begin(), g.values.end(),
            [guid = node.guid](const Value& v) { return v.node == guid; }), g.values.end());
        to_delete.push_back(node.guid);
    }

    void IGE_MaterialEditor::disconnectNode(const Node& node)
    {
        auto& g = *graph;
        g.links.erase(std::remove_if(g.links.begin(), g.links.end(),
            [guid = node.guid](const Link& link) { return link.node_in == guid || link.node_out == guid; }), g.links.end());
    }



    struct node_item { vector<node_item> items; string name; };

    static node_item* context_menu_node_item(node_item& item, std::vector<node_item*>& stack)
    {
        if (item.items.size())
        {
            auto folder_name = item.name;
            if (folder_name == "master") // skip master nodes
                return nullptr;
            if (stack.empty() || ImGui::TreeNodeEx(folder_name.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
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
            if (ImGui::MenuItem(item.name.c_str()))
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
            std::sort(item->items.begin(), item->items.end(), [](node_item& a, node_item& b) { return a.name < b.name; });
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



    IGE_MaterialEditor::IGE_MaterialEditor()
        : IGE_IWindow("Material Editor", true, ImVec2{ 600,300 }, ImVec2{ 150,150 })
    {
        window_flags |= ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_MenuBar;
    }

    void IGE_MaterialEditor::Initialize()
    {
        canvas.colors[ImNodes::ColNodeBg] = ImColor(0.25f, 0.25f, 0.3f);
        canvas.colors[ImNodes::ColNodeActiveBg] = ImColor(0.25f, 0.25f, 0.3f);
        canvas.style.curve_thickness = 2.5f;

        graph = Core::GetResourceManager().Create<Graph>(
            Core::GetSystem<FileSystem>().GetFile("/assets/materials/test.mat"));
    }

    void IGE_MaterialEditor::BeginWindow()
    {
    }

    void IGE_MaterialEditor::Update()
    {
        if (!is_open || ImGui::IsWindowCollapsed())
            return;

        if (!graph)
        {
            //graph = Core::GetResourceManager().Create<Graph>();
            //addNode("master\\PBR", { 500.0f, 200.0f });
            //g.master_node = g.nodes.begin()->first;
            //Core::GetSystem<SaveableResourceManager>().Save(graph);
            return;
        }

        auto& g = *graph;


        if (ImGui::BeginMenuBar())
        {
            if (ImGui::Button("Compile"))
            {
                g.Compile();
                Core::GetSystem<SaveableResourceManager>().Save(graph);
            }
            ImGui::EndMenuBar();
        }


        show_params_window();


        auto window_pos = ImGui::GetWindowPos();

        ImGui::SetWindowFontScale(1.0f);
        ImGui::SetNextWindowSizeConstraints(ImVec2{ 0, 0 }, ImVec2{ 1000, 320 });
        if (!ImGui::IsMouseDragPastThreshold(1) && ImGui::BeginPopupContextWindow())
        {
            auto str = draw_nodes_context_menu();
            if (str.size())
            {
                auto pos = (ImGui::GetWindowPos() - window_pos - canvas.offset) / canvas.zoom;
                addNode(str, r_cast<vec2&>(pos));
                // pos = windowpos + nodepos * zoom + offset
                // nodepos = (screenpos - offset - windowpos) / zoom
            }
            ImGui::EndPopup();
        }

        ImNodes::BeginCanvas(&canvas);

        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.0f);
        for (auto& value : g.values)
            drawValue(value);
        for (auto& node : g.nodes)
            drawNode(node.second);
        for(auto& guid : to_delete)
            g.nodes.erase(guid);
        ImGui::PopStyleVar();

        auto connection_col_active = canvas.colors[ImNodes::ColConnectionActive];
        {
            Node* node;
            const char* title;
            int kind;

            if (ImNodes::GetPendingConnection(r_cast<void**>(&node), &title, &kind))
            {
                canvas.colors[ImNodes::ColConnectionActive] = type_colors[std::abs(kind)];
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
                int slot_out = (int)NodeTemplate::GetTable().at(node_out->name).GetSlotIndex(title_out);

                // check if any value attached to input slot; remove it
                for (auto iter = g.values.begin(); iter != g.values.end(); ++iter)
                {
                    if (iter->node == node_in->guid && iter->slot == slot_in)
                    {
                        g.values.erase(iter);
                        break;
                    }
                }
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

                node_in->input_slots[slot_in].type = node_out->output_slots[slot_out - node_out->input_slots.size()].type;

                // resolve new node_in output type
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

        for (auto& link : g.links)
        {
            auto& node_out = g.nodes[link.node_out];
            auto& node_in = g.nodes[link.node_in];
            auto col = canvas.colors[ImNodes::ColConnection];
            canvas.colors[ImNodes::ColConnection] = type_colors[std::abs(node_in.input_slots[link.slot_in].type)];
            ImNodes::Connection(&node_in, NodeTemplate::GetTable().at(node_in.name).names[link.slot_in].c_str(),
                                &node_out, NodeTemplate::GetTable().at(node_out.name).names[link.slot_out].c_str());
            canvas.colors[ImNodes::ColConnection] = col;
        }

        ImNodes::EndCanvas();
        canvas.colors[ImNodes::ColConnectionActive] = connection_col_active;
    }

    void IGE_MaterialEditor::show_params_window()
    {
        ImGui::SetWindowFontScale(1.0f);
        if (ImGui::IsWindowFocused())
            ImGui::SetNextWindowFocus();
        ImGui::SetNextWindowPos(ImGui::GetCursorScreenPos());
        ImGui::SetNextWindowSizeConstraints(ImVec2(150, 200), ImGui::GetContentRegionAvail());

        if (ImGui::Begin("Parameters##MaterialEditor_Parameters", 0, ImGuiWindowFlags_NoMove))
        {
            if (ImGui::Button("Add Parameter"))
            {
                graph->parameters.emplace_back(Parameter{ "NewParameter", Guid::Make(), ValueType::FLOAT, "0" });
            }

            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(1.0f, 1.0f));

            static char buf[32];
            int id = 0;
            for (auto& param : graph->parameters)
            {
                auto* draw_list = ImGui::GetWindowDrawList();
                draw_list->ChannelsSplit(2);
                draw_list->ChannelsSetCurrent(1);

                ImGui::PushID(++id);
                ImGui::BeginGroup();
                ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(2.0f, 2.0f));
                ImGui::BeginGroup();

                strcpy_s(buf, param.name.c_str());
                if (ImGui::InputText("Name", buf, 32))
                {
                    param.name = buf;
                }

                if (ImGui::BeginCombo("Type", string{ param.type.to_string() }.c_str()))
                {
                    for (auto& sv : ValueType::names)
                    {
                        if (ImGui::Selectable(string{ sv }.c_str()))
                        {
                            param.type = ValueType::from_string(sv);
                            param.default_value = default_value(param.type);
                        }
                    }
                    ImGui::EndCombo();
                }

                switch (param.type)
                {
                case ValueType::FLOAT:
                {
                    float f = std::stof(param.default_value);
                    if (ImGui::DragFloat("Default", &f, 0.01f))
                    {
                        param.default_value = std::to_string(f);
                    }
                    break;
                }
                case ValueType::VEC2:
                {
                    float f[2]{ 0, 0 };
                    std::smatch matches;
                    if (std::regex_match(param.default_value, matches, std::regex("([\\d\\.\\-]+),([\\d\\.\\-]+)")))
                    {
                        f[0] = std::stof(matches[1]);
                        f[1] = std::stof(matches[2]);
                    }
                    if (ImGui::DragFloat3("Default", f, 0.01f))
                    {
                        param.default_value = std::to_string(f[0]) + ',' + std::to_string(f[1]);
                    }
                    break;
                }
                case ValueType::VEC3:
                {
                    float f[3]{ 0, 0, 0 };
                    std::smatch matches;
                    if (std::regex_match(param.default_value, matches, std::regex("([\\d\\.\\-]+),([\\d\\.\\-]+),([\\d\\.\\-]+)")))
                    {
                        f[0] = std::stof(matches[1]);
                        f[1] = std::stof(matches[2]);
                        f[2] = std::stof(matches[3]);
                    }
                    if (ImGui::DragFloat3("Default", f, 0.01f))
                    {
                        param.default_value = std::to_string(f[0]) + ',' + std::to_string(f[1]) + ',' + std::to_string(f[2]);
                    }
                    break;
                }
                default:
                    break;
                }

                ImGui::EndGroup();
                ImGui::EndGroup();
                ImGui::PopID();

                draw_list->ChannelsSetCurrent(0);
                auto min = ImGui::GetItemRectMin();
                auto max = ImVec2(min.x + ImGui::GetWindowContentRegionWidth(), ImGui::GetItemRectMax().y) + ImVec2(2.0f, 2.0f);
                draw_list->AddRect(min, max, canvas.colors[ImNodes::ColNodeBg]);

                draw_list->ChannelsMerge();
            }

            ImGui::PopStyleVar();
        }
        ImGui::End();
    }

}