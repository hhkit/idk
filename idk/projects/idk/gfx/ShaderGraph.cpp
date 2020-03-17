#include "stdafx.h"
#include "ShaderGraph.h"
#include <res/ResourceHandle.inl>
#include <gfx/MeshRenderer.h>
#include <anim/SkinnedMeshRenderer.h>
#include <gfx/ShaderTemplate.h>
#include <gfx/ShaderProgram.h>
#include <gfx/ShaderGraph_helpers.h>
#include <serialize/text.inl>
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
        hash_table<string, string> non_param_textures{};
        int tex_counter = 0;
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

    static string wrap_vec_ctor(const Node& node, int slot_index, const string& args)
    {
        if (slot_index < node.input_slots.size())
        {
            auto t = node.input_slots[slot_index].type;
            if (t != ValueType::SAMPLER2D)
            {
                string wrapped = t.to_string();
                make_lowercase(wrapped);
                wrapped += '(';
                wrapped += args;
                wrapped += ')';
                return wrapped;
            }
        }
        else if (slot_index < node.input_slots.size() + node.output_slots.size())
        {
            auto t = node.output_slots[slot_index - node.input_slots.size()].type;
            if (t != ValueType::SAMPLER2D)
            {
                string wrapped = t.to_string();
                make_lowercase(wrapped);
                wrapped += '(';
                wrapped += args;
                wrapped += ')';
                return wrapped;
            }
        }
        return args;
    }

    static void replace_variables(string& code, int slot_index, const string& replacement)
    {
        string to_find = '{' + serialize_text(slot_index) + '}';
        size_t offset = 0;

        while ((offset = code.find(to_find, offset)) != string::npos)
            code = code.replace(offset, to_find.size(), replacement);
    }

    static void resolve_conditionals(string& code, const Node& node)
    {
        std::regex regex{ "\\?(\\d+):(\\w+)\\{(.*)\\}" };
        std::smatch sm;

        std::string _code = code;
        while (std::regex_search(_code, sm, regex))
        {
            const auto& index = std::stoi(sm[1]);
            string type = sm[2].str();
            const auto& inner = sm[3];

            if (index < node.input_slots.size())
            {
                if (node.input_slots[index].type == ValueType::from_string(make_uppercase(type)))
                    _code.replace(sm.position(), sm.length(), inner.str());
                else
                    _code.replace(sm.position(), sm.length(), "");
            }
            else if(index < node.input_slots.size() + node.output_slots.size())
            {
                if (node.output_slots[index - node.input_slots.size()].type == ValueType::from_string(make_uppercase(type)))
                    _code.replace(sm.position(), sm.length(), inner.str());
                else
                    _code.replace(sm.position(), sm.length(), "");
            }
            else // conditional
            {
                auto control_index = index - node.input_slots.size() - node.output_slots.size();
                size_t pos = node.control_values.find('|', 0), start = 0;
                while (control_index && pos != string::npos)
                {
                    start = pos + 1;
                    pos = node.control_values.find('|', start);
                    --control_index;
                }
                if (control_index == 0 && node.control_values.substr(start, pos - start) == type)
                    _code.replace(sm.position(), sm.length(), inner.str());
                else
                    _code.replace(sm.position(), sm.length(), "");
            }
        }
        code = _code;
    }

    static string resolve_node(const Node& node, compiler_state& state)
    {
        string code = "";
        string code_from_input = "";

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
                code += " {" + serialize_text(node.input_slots.size() + i) + "};\n";
            }

            auto& tpl = NodeTemplate::GetTable().at(node.name);

            // add the code, then replace the output variable names
            code += tpl.code;
            resolve_conditionals(code, node); // resolve conditionals based on types or dropdown (?<index>:<type>{...})
            for (int i = 0; i < node.output_slots.size(); ++i)
            {
                replace_variables(code, static_cast<int>(node.input_slots.size() + i), var_name(state.slot_counter));
                state.resolved_outputs.emplace(NodeSlot{ node.guid, i }, var_name(state.slot_counter++));
            }

			const auto num_slots = static_cast<int>(tpl.signatures[0].ins.size() + tpl.signatures[0].outs.size());
            if (tpl.names.size() > num_slots) // have custom controls
            {
                string str = node.control_values;
                for (int i = num_slots; i < tpl.names.size(); ++i)
                {
                    string next_value = "";
                    if (str.size())
                    {
                        const auto pos = str.find('|');
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
            auto param_index = std::stoi(uniform_name.data() + 1);
            auto& param = state.graph.parameters[param_index];
            if (param.type == ValueType::SAMPLER2D)
            {
                uniform_name = "_uTex[";
                uniform_name += serialize_text(state.tex_counter++);
                uniform_name += ']';
                state.resolved_outputs.emplace(NodeSlot{ node.guid, 0 }, uniform_name);
            }
            else
            {
                uniform_name.replace(0, 1, "_u");
                state.resolved_outputs.emplace(NodeSlot{ node.guid, 0 },
                                               "_ub" + serialize_text<char>(param.type) + "." + uniform_name);
            }
            state.uniforms[param_index] = std::make_pair(uniform_name, param.type);
        }

        for (int i = 0; i < node.input_slots.size(); ++i)
        {
            auto iter = state.inputs_to_outputs.find({ node.guid, i });

            if (iter == state.inputs_to_outputs.end()) // no connection, use slot's value
            {
                const auto& value = node.input_slots[i].value;

                string replacement;
                if (node.input_slots[i].type == ValueType::SAMPLER2D)
                {
                    // can't connect textures directly, we need to go through uniforms
                    auto uniform_name = "_uTex[" + std::to_string(state.tex_counter++) + ']';
                    state.non_param_textures[uniform_name] = node.input_slots[i].value;
                    replacement = uniform_name;
                    replace_variables(code, i, replacement);
                }
                else
                {
                    replacement = node.input_slots[i].type.to_string();
                    make_lowercase(replacement);
                    replacement += '(' + value + ')';
                    replace_variables(code, i, wrap_vec_ctor(node, i, replacement));
                }

                continue;
            }

            // else, connected to another node
            auto& link = iter->second;
            auto& node_out = state.graph.nodes.at(link->node_out);

            auto resolved_iter = state.resolved_outputs.find({ node_out.guid, link->slot_out - s_cast<int>(node_out.input_slots.size()) });
            if (resolved_iter != state.resolved_outputs.end())
            {
                replace_variables(code, i, wrap_vec_ctor(node, i, resolved_iter->second));
            }
            else
            {
                code_from_input += resolve_node(state.graph.nodes.at(link->node_out), state);
                code_from_input += '\n';
                resolved_iter = state.resolved_outputs.find({ node_out.guid, link->slot_out - s_cast<int>(node_out.input_slots.size()) });
                assert(resolved_iter != state.resolved_outputs.end());
                replace_variables(code, i, wrap_vec_ctor(node, i, resolved_iter->second));
            }
        }

        return code_from_input + '\n' + code;
    }


	static UniformInstanceValue to_uniform_instance_value(const Parameter& param)
	{
		switch (param.type)
		{
		case ValueType::FLOAT: return std::stof(param.default_value);
		case ValueType::VEC2: return helpers::parse_vec2(param.default_value);
		case ValueType::VEC3: return helpers::parse_vec3(param.default_value);
		case ValueType::VEC4: return helpers::parse_vec4(param.default_value);
		case ValueType::SAMPLER2D: return helpers::parse_sampler2d(param.default_value);
		default: throw;
		}
	}


    void Graph::Compile()
    {
        compiler_state state{ *this };
        state.uniforms.resize(parameters.size());

        for (auto& link : links)
            state.inputs_to_outputs.emplace(NodeSlot{ link.node_in, link.slot_in }, &link);

        string code = resolve_node(nodes.at(master_node), state);
        string uniforms_str = "";
        uniforms.clear();

        array<string, ValueType::count + 1> uniform_blocks;

        // add block for time
        uniform_blocks[0] = "U_LAYOUT(1, 0) uniform BLOCK(_UB0) { float time;";

        for (const auto& [uniform_name, uniform_type] : state.uniforms)
        {
            if (uniform_name.empty() || uniform_type == ValueType::SAMPLER2D)
                continue;

            auto& block = uniform_blocks[uniform_type];

            if (uniform_type == ValueType::SAMPLER2D)
                continue;

            string str{ uniform_type.to_string() };
            string typestr = make_lowercase(str);

            if (block.empty())
            {
                block += "U_LAYOUT(1, ";
                block += serialize_text<char>(uniform_type);
                block += ") uniform BLOCK(_UB";
                block += serialize_text<char>(uniform_type);
                block += ")\n{\n";
            }
            block += "  ";
            block += typestr;
            block += ' ';
            block += uniform_name;
            block += ";\n";
        }

        for (const auto& [uniform_name, guid_str] : state.non_param_textures)
        {
            hidden_uniforms.emplace_back(UniformInstance{ uniform_name, RscHandle<Texture>(Guid(guid_str)) });
        }

        for (size_t i = 0; i < uniform_blocks.size(); ++i)
        {
            if (uniform_blocks[i].empty() || i == ValueType::SAMPLER2D)
                continue;

            uniforms_str += uniform_blocks[i];
            uniforms_str += "} _ub";
            uniforms_str += serialize_text(i);
            uniforms_str += ";\n";
        }

        if (state.tex_counter > 0) // U_LAYOUT(3, 8) uniform sampler2D _uTex[count];
            uniform_blocks[ValueType::SAMPLER2D] = "S_LAYOUT(2, " + std::to_string(ValueType::SAMPLER2D) +
            ") uniform sampler2D _uTex[" + std::to_string(state.tex_counter) + "];\n";
        uniforms_str += uniform_blocks[ValueType::SAMPLER2D];

        int param_index = -1;
        for (const auto& [uniform_name, uniform_type] : state.uniforms)
        {
            ++param_index;
            if (uniform_name.empty())
                continue;

            if (uniform_type == ValueType::SAMPLER2D)
                uniforms.emplace(parameters[param_index].name,
                    UniformInstance{ uniform_name, to_uniform_instance_value(parameters[param_index]) });
            else
                uniforms.emplace(parameters[param_index].name,
                    UniformInstance{ "_ub" + serialize_text<char>(uniform_type) + '.' + uniform_name,
                                     to_uniform_instance_value(parameters[param_index]) });
        }

        auto shader_template = GetTemplate()->Instantiate(uniforms_str, code);
        if (_shader_program.guid == Guid{})
            _shader_program = Core::GetResourceManager().Create<ShaderProgram>();
        else if (!_shader_program)
        {
            _shader_program = Core::GetResourceManager().LoaderCreateResource<ShaderProgram>(_shader_program.guid);
        }
        auto& program = *_shader_program;
        program.Name(string{ this->Name() }+"_" + string{ GetTemplate()->Name() });

        program.BuildShader(ShaderStage::Fragment, shader_template);
    }

}