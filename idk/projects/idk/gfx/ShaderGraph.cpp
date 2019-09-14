#include "stdafx.h"
#include "ShaderGraph.h"
#include <gfx/Material.h>
#include <gfx/MeshRenderer.h>
#include <regex>

namespace idk::shadergraph
{
    struct NodeSlot
    {
        const Guid node;
        const int slot;
        bool operator==(const NodeSlot& other) const
        {
            return node == other.node && slot == other.slot;
        }
    };

    struct NodeSlotHasher
    {
        size_t operator()(const NodeSlot& nodeslot) const
        {
            return std::hash<Guid>()(nodeslot.node) + nodeslot.slot;
        }
    };

    struct compiler_state
    {
        const Graph& graph;
        hash_table<NodeSlot, variant<const Link*, const Value*>, NodeSlotHasher> inputs_to_outputs{};
        hash_table<NodeSlot, int, NodeSlotHasher> resolved_outputs{};
        int slot_counter = 0;
    };

    static string& make_uppercase(string& str)
    {
        for (char& c : str)
            c = static_cast<char>(toupper(c));
        return str;
    }
    static string& make_lowercase(string& str)
    {
        for (char& c : str)
            c = static_cast<char>(tolower(c));
        return str;
    }

    static string var_name(int counter)
    {
        return "_v" + std::to_string(counter);
    }

    static void replace_variables(string& code, int slot_index, const string& replacement)
    {
        string to_find = '{' + std::to_string(slot_index) + '}';
        size_t offset = 0;
        while ((offset = code.find(to_find, offset)) != string::npos)
            code = code.replace(offset, to_find.size(), replacement);
    }

    static void resolve_conditionals(string& code, const Node& node)
    {
        std::regex regex{ "\\?(\\d+):(float|vec2|vec3|vec4|mat2|mat3|mat4|sampler2D)\\{(.*)\\}" };
        std::smatch sm;

        while (std::regex_search(code, sm, regex))
        {
            const auto& index = std::stoi(sm[1]);
            auto type = sm[2].str();
            const auto& inner = sm[3];

            if (index < node.input_slots.size())
            {
                if (node.input_slots[index].type == ValueType::from_string(make_uppercase(type)))
                    code.replace(sm.position(), sm.length(), inner);
                else
                    code.replace(sm.position(), sm.length(), "");
            }
            else
            {
                if (node.output_slots[index - node.input_slots.size()].type == ValueType::from_string(make_uppercase(type)))
                    code.replace(sm.position(), sm.length(), inner);
                else
                    code.replace(sm.position(), sm.length(), "");
            }
        }
    }

    static string resolve_node(const Node& node, compiler_state& state)
    {
        string code = "";

        // add "<type> <varname>;" for every output slots
        for (int i = 0; i < node.output_slots.size(); ++i)
        {
            if (node.output_slots[i].type == ValueType::SAMPLER2D)
                code += "sampler2D";
            else
            {
                string str{ node.output_slots[i].type.to_string() };
                code += make_lowercase(str);
            }
            code += " {" + std::to_string(node.input_slots.size() + i) + "};\n";
        }

        // add the code, then replace the output variable names
        code += NodeTemplate::GetTable().at(node.name).code;
        resolve_conditionals(code, node); // resolve conditionals based on types (?<index>:<type>{...})
        for (int i = 0; i < node.output_slots.size(); ++i)
        {
            replace_variables(code, static_cast<int>(node.input_slots.size() + i), var_name(state.slot_counter));
            state.resolved_outputs.emplace(NodeSlot{ node.guid, i }, state.slot_counter++);
        }

        for (int i = 0; i < node.input_slots.size(); ++i)
        {
            auto iter = state.inputs_to_outputs.find({ node.guid, i });
            assert(iter != state.inputs_to_outputs.end()); // i fucked up

            auto& output = iter->second;
            if (output.index() == 0) // connected to another node
            {
                auto& link = std::get<0>(output);
                auto& node_out = state.graph.nodes.at(link->node_out);

                auto resolved_iter = state.resolved_outputs.find({ node_out.guid, link->slot_out - s_cast<int>(node_out.input_slots.size()) });
                if (resolved_iter != state.resolved_outputs.end())
                {
                    replace_variables(code, i, var_name(resolved_iter->second));
                }
                else
                {
                    code = resolve_node(state.graph.nodes.at(link->node_out), state) + "\n" + code;
                    resolved_iter = state.resolved_outputs.find({ node_out.guid, link->slot_out - s_cast<int>(node_out.input_slots.size()) });
                    assert(resolved_iter != state.resolved_outputs.end());
                    replace_variables(code, i, var_name(resolved_iter->second));
                }
            }
            else // connected to value
            {
                auto& value = std::get<1>(output);
                std::string replacement;
                if (node.input_slots[i].type == ValueType::SAMPLER2D)
                    replacement = "sampler2D";
                else
                {
                    replacement = node.input_slots[i].type.to_string();
                    make_lowercase(replacement);
                }
                replacement += '(' + value->value + ')';
                replace_variables(code, i, replacement);
            }
        }

        return code;
    }

    void Graph::Compile()
    {
        // todo: handle added uniforms

        compiler_state state{ *this };

        for (auto& link : links)
            state.inputs_to_outputs.emplace(NodeSlot{ link.node_in, link.slot_in }, &link);
        for (auto& value : values)
            state.inputs_to_outputs.emplace(NodeSlot{ value.node, value.slot }, &value);

        string code = resolve_node(nodes.at(master_node), state);

		auto shader_template = Core::GetResourceManager().LoadFile("/assets/shader/pbr_forward.tmpt")[0].As<ShaderTemplate>();
		auto h_mat = Core::GetResourceManager().Create<Material>();
		h_mat->BuildShader(shader_template, "", code);

		for (auto& renderer : GameState::GetGameState().GetObjectsOfType<MeshRenderer>())
		{
			renderer.material_instance.material = h_mat;
		}
    }

}