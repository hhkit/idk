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
        hash_table<NodeSlot, const Link*, NodeSlotHasher> inputs_to_outputs{};
        hash_table<NodeSlot, string, NodeSlotHasher> resolved_outputs{};
        vector<std::pair<string, ValueType>> uniforms{};
        vector<string> non_param_textures{};
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

        if (node.name[0] != '$') // is not a param node
        {
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

            auto& tpl = NodeTemplate::GetTable().at(node.name);

            // add the code, then replace the output variable names
            code += tpl.code;
            resolve_conditionals(code, node); // resolve conditionals based on types (?<index>:<type>{...})
            for (int i = 0; i < node.output_slots.size(); ++i)
            {
                replace_variables(code, static_cast<int>(node.input_slots.size() + i), var_name(state.slot_counter));
                state.resolved_outputs.emplace(NodeSlot{ node.guid, i }, var_name(state.slot_counter++));
            }

            size_t num_slots = tpl.signatures[0].ins.size() + tpl.signatures[0].outs.size();
            if (tpl.names.size() > num_slots) // have custom controls
            {
                string str = node.control_values;
                for (int i = num_slots; i < tpl.names.size(); ++i)
                {
                    string next_value = "";
                    if (str.size())
                    {
                        auto pos = str.find('|');
                        next_value = str.substr(0, pos);
                        if (pos != string::npos)
                            str.erase(0, pos + 1);
                        else
                            str.clear();
                    }
                    replace_variables(code, i, next_value);
                }
            }
        }
        else // param node
        {
            auto uniform_name = node.name;
            auto& param = state.graph.parameters[std::stoi(uniform_name.data() + 1)];
            uniform_name.replace(0, 1, "_u");
            state.resolved_outputs.emplace(NodeSlot{ node.guid, 0 }, "_ub" + std::to_string(param.type) + "." + uniform_name); // eg. _ub0._u0
            state.uniforms.emplace_back(std::make_pair(uniform_name, param.type));
        }

        for (int i = 0; i < node.input_slots.size(); ++i)
        {
            auto iter = state.inputs_to_outputs.find({ node.guid, i });

            if (iter == state.inputs_to_outputs.end()) // no connection, use slot's value
            {
                const auto& value = node.input_slots[i].value;

                std::string replacement;
                if (node.input_slots[i].type == ValueType::SAMPLER2D)
                {
                    // can't connect textures directly, we need to go through uniforms
                    auto uniform_name = "_c" + std::to_string(state.non_param_textures.size());
                    state.non_param_textures.push_back(uniform_name);
                    replacement = "_ub" + std::to_string(ValueType::SAMPLER2D) + '.' + uniform_name;
                    replace_variables(code, i, replacement);
                }
                else
                {
                    replacement = node.input_slots[i].type.to_string();
                    make_lowercase(replacement);
                    replacement += '(' + value + ')';
                    replace_variables(code, i, replacement);
                }

                continue;
            }

            // else, connected to another node
            auto& link = iter->second;
            auto& node_out = state.graph.nodes.at(link->node_out);

            auto resolved_iter = state.resolved_outputs.find({ node_out.guid, link->slot_out - s_cast<int>(node_out.input_slots.size()) });
            if (resolved_iter != state.resolved_outputs.end())
            {
                replace_variables(code, i, resolved_iter->second);
            }
            else
            {
                code = resolve_node(state.graph.nodes.at(link->node_out), state) + "\n" + code;
                resolved_iter = state.resolved_outputs.find({ node_out.guid, link->slot_out - s_cast<int>(node_out.input_slots.size()) });
                assert(resolved_iter != state.resolved_outputs.end());
                replace_variables(code, i, resolved_iter->second);
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

        string code = resolve_node(nodes.at(master_node), state);
        string uniforms = "";

        if (state.uniforms.size())
        {
            array<string, ValueType::count> uniform_blocks;

            for (const auto& [uniform_name, uniform_type] : state.uniforms)
            {
                string typestr = "";
                if (uniform_type == ValueType::SAMPLER2D)
                    typestr = "sampler2D";
                else
                {
                    string str{ uniform_type.to_string() };
                    typestr = make_lowercase(str);
                }

                auto& block = uniform_blocks[uniform_type];

                if (block.empty())
                {
                    block += "U_LAYOUT(3, ";
                    block += std::to_string(uniform_type);
                    block += ") uniform BLOCK(_UB";
                    block += uniform_type.to_string();
                    block += ")\n{\n";
                }
                block += "  ";
                block += typestr;
                block += ' ';
                block += uniform_name;
                block += ";\n";
            }

            for(const auto& uniform_name : state.non_param_textures)
            {
                auto& block = uniform_blocks[ValueType::SAMPLER2D];
                if (block.empty())
                {
                    block += "U_LAYOUT(3, ";
                    block += std::to_string(ValueType::SAMPLER2D);
                    block += ") uniform BLOCK(_UB";
                    block += ValueType(ValueType::SAMPLER2D).to_string();
                    block += ")\n{\n";
                }
                block += "  sampler2D ";
                block += uniform_name;
                block += ";\n";
            }

            for (size_t i = 0; i < uniform_blocks.size(); ++i)
            {
                if (uniform_blocks[i].empty())
                    continue;

                uniforms += uniform_blocks[i];
                uniforms += "} _ub";
                uniforms += std::to_string(i);
                uniforms += ";\n";
            }
        }

		auto shader_template = *Core::GetResourceManager().Load<ShaderTemplate>("/assets/shader/pbr_forward.tmpt");
		auto h_mat = Core::GetResourceManager().Create<Material>();
		h_mat->BuildShader(shader_template, uniforms, code);

		for (auto& renderer : GameState::GetGameState().GetObjectsOfType<MeshRenderer>())
		{
			renderer.material_instance.material = h_mat;
		}
    }

}