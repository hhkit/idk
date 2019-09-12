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
        if (ImNodes::BeginNode(&node, r_cast<ImVec2*>(&node.position), &node.selected))
        {
            auto& tpl = NodeTemplate::GetTable().at(node.name);

            auto slash_pos = node.name.find_last_of('\\');
            auto title = node.name.c_str() + (slash_pos == std::string::npos ? 0 : slash_pos + 1);
            auto title_size = ImGui::CalcTextSize(title);
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
            ImGui::Text(title);

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
        : IGE_IWindow("Material Editor", true, ImVec2{ 300,600 }, ImVec2{ 150,150 })
    {
    }

    void IGE_MaterialEditor::Initialize()
    {
        canvas.colors[ImNodes::ColNodeBg] = ImColor(0.25f, 0.25f, 0.3f);
        canvas.colors[ImNodes::ColNodeActiveBg] = ImColor(0.25f, 0.25f, 0.3f);
        canvas.style.curve_thickness = 2.5f;
    }

    void IGE_MaterialEditor::BeginWindow()
    {
    }

    void IGE_MaterialEditor::Update()
    {
        if (!graph)
        {
            //auto file = Core::GetSystem<FileSystem>().GetFile("/assets/ShaderGraph.mat");
            //if (file)
            //    graph = Core::GetResourceManager().Create<Graph>(file);
            //else
            //    graph = Core::GetResourceManager().Create<Graph>();
            //addNode("master\\Unlit", { 800.0f, 300.0f });
            //graph->master_node = graph->nodes.begin()->first;
            //Core::GetSystem<SaveableResourceManager>().Save(graph);
            return;
        }

        if (ImGui::Button("Compile"))
        {
            graph->Compile();
        }

        auto window_pos = ImGui::GetWindowPos();



        ImNodes::BeginCanvas(&canvas);

        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.0f);
        for (auto& value : graph->values)
        {
            drawValue(value);
        }
        for (auto& node : graph->nodes)
        {
            drawNode(node.second);
        }
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
                for (auto iter = graph->values.begin(); iter != graph->values.end(); ++iter)
                {
                    if (iter->node == node_in->guid && iter->slot == slot_in)
                    {
                        graph->values.erase(iter);
                        break;
                    }
                }
                // check if any link inputs into input slot (cannot have multiple)
                for (auto iter = graph->links.begin(); iter != graph->links.end(); ++iter)
                {
                    if (iter->node_in == node_in->guid && iter->slot_in == slot_in)
                    {
                        graph->links.erase(iter);
                        break;
                    }
                }

                graph->links.push_back({ node_out->guid, node_in->guid, slot_out, slot_in });

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

        for (auto& link : graph->links)
        {
            auto& node_out = graph->nodes[link.node_out];
            auto& node_in = graph->nodes[link.node_in];
            auto col = canvas.colors[ImNodes::ColConnection];
            canvas.colors[ImNodes::ColConnection] = type_colors[std::abs(node_in.input_slots[link.slot_in].type)];
            ImNodes::Connection(&node_in, NodeTemplate::GetTable().at(node_in.name).names[link.slot_in].c_str(),
                                &node_out, NodeTemplate::GetTable().at(node_out.name).names[link.slot_out].c_str());
            canvas.colors[ImNodes::ColConnection] = col;
        }



        //const ImGuiIO& io = ImGui::GetIO();
        if (ImGui::IsMouseReleased(1) && ImGui::IsWindowHovered() && !ImGui::IsMouseDragPastThreshold(1))
        {
            ImGui::FocusWindow(ImGui::GetCurrentWindow());
            ImGui::OpenPopup("NodesContextMenu");
        }

        if (ImGui::IsPopupOpen("NodesContextMenu"))
        {
            ImGui::SetWindowFontScale(1.0f);
            ImGui::SetNextWindowSizeConstraints(ImVec2{ 0, 0 }, ImVec2{ 1000, 320 });
        }
        if (ImGui::BeginPopup("NodesContextMenu"))
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



        ImNodes::EndCanvas();
        canvas.colors[ImNodes::ColConnectionActive] = connection_col_active;
    }

}