#include "stdafx.h"
#include "yaml.h"

namespace idk::yaml
{

    enum _style { block_style, flow_style };

    struct dumper
    {
        string output;
        vector<int> indent_stack{ 0 };
        _style style = block_style;
        bool is_new_line = true;

        void indent() { indent_stack.push_back(indent_stack.back() + 2); }
        void unindent() { indent_stack.pop_back(); }

        void write_null()
        {
            if (style != block_style) 
                write('~'); 
        }
        void write(char c)
        {
            if (is_new_line)
                output.append(indent_stack.back(), ' ');
            output += c;
            is_new_line = false;
        }
        void write(string_view sv)
        {
            if (is_new_line)
                output.append(indent_stack.back(), ' ');
            output += sv;
            is_new_line = false;
        }

        // handles quoting/escaping if needed
        void write_scalar(const node& _node)
        {
            bool must_escape = false;
            bool has_single_quotes = false;
            bool has_flow_braces = false;
            bool has_commas = false;
            const auto& scalar = _node.as_scalar();

            if (scalar.empty())
            {
                write("\"\""); // "" -> empty string
                return;
            }

            for (size_t i = 0; i < scalar.size(); ++i)
            {
                switch (scalar[i])
                {
                case '\'':
                    has_single_quotes = true; break;
                case '\0': case '\t': case '\n': case '\r': 
                    must_escape = true; break;
                case '[': case ']': case '{': case '}':
                    has_flow_braces = true; break;
                case ',':
                    has_commas = true; break;
                default: break;
                }
            }

            if (!must_escape)
            {
                // "xyz" => '"xyz"' (so "" dont get lost during parse)
                if ((scalar.front() == '\"' && scalar.back() == '\"') ||
                    (style == flow_style && (has_flow_braces || has_commas))) // ['a[]a'] or ['a,a'] have to quote-enclose flow braces / commas
                {
                    write('\'');
                    if (has_single_quotes)
                    {
                        // have to escape single quotes
                        string str;
                        for (size_t i = 0; i < scalar.size(); ++i)
                        {
                            if (scalar[i] == '\'')
                                str += "''";
                            else
                                str += scalar[i];
                        }
                        write(str);
                    }
                    else
                        write(scalar);
                    write('\'');
                }
                else
                    write(scalar);
            }
            else
            {
                string str = "\"";
                for (size_t i = 0; i < scalar.size(); ++i)
                {
                    switch (scalar[i])
                    {
                    case '\\': str += "\\\\"; break;
                    case '\"': str += "\\\""; break;
                    case '\0': str += "\\0"; break;
                    case '\t': str += "\\t"; break;
                    case '\n': str += "\\n"; break;
                    case '\r': str += "\\r"; break;
                    default: str += scalar[i]; break;
                    }
                }
                str += '"';
                write(str);
            }
        }
        void write_tag(const node& _node)
        {
            if (!_node.has_tag())
                return;
            write('!');
            write(_node.tag());
            write(' ');
        }

        void new_line() 
        {
            // max 1 lf
            if(output.size() && output.back() != '\n')
                output += '\n';
            is_new_line = true; 
        }

        bool should_flow(const node& _node)
        {
            if (_node.size() > 3) // to make stuff more readable, max 3 items
                return false;
            if (_node.type() == type::null || _node.type() == type::scalar)
                return false;

            // should have height of <= 1

            if (_node.size() == 0)
                return true;
            else if (_node.type() == type::sequence)
            {
                for (const auto& item : _node)
                {
                    if (item.size() >= 1)
                        return false;
                }
            }
            else if (_node.type() == type::mapping)
            {
                for (const auto& item : _node.as_mapping())
                {
                    if (item.second.size() >= 1)
                        return false;
                }
            }

            return true;
        }

        void dump(const node& _node)
        {
            switch (_node.type())
            {
            case type::null:
                write_tag(_node);
                write_null(); 
                break;

            case type::scalar:
                write_tag(_node); 
                write_scalar(_node);
                break;

            case type::sequence:
            {
                assert(style == block_style); // should never have nested flow styles, see should_flow()
                const auto& seq = _node.as_sequence();
                if (seq.empty())
                    write("[]");
                else if (should_flow(_node))
                {
                    style = flow_style;
                    write('[');
                    for (const auto& item : seq)
                    {
                        dump(item);
                        write(", ");
                    }
                    output.pop_back();
                    output.back() = ']';
                    style = block_style;
                }
                else
                {
                    for (const auto& item : seq)
                    {
                        write("- ");
                        indent();
                        // for inner blocks, write tag before going to next line
                        if (item.type() == type::mapping)
                        {
                            if (item.has_tag())
                            {
                                write_tag(item);
                                new_line();
                            }

                            // if only 1 key-value pair in mapping, dont bother flowing
                            if (should_flow(item) && item.size() == 1)
                            {
                                auto& [key, val] = *item.as_mapping().begin();
                                write(key);
                                write(": ");
                                dump(val);
                                unindent();
                                new_line();
                                continue; // next item in seq
                            }
                        }
                        else if (item.type() == type::sequence && !should_flow(item)) // block seq in block seq is weird af
                        {
                            write_tag(item);
                            new_line();
                        }
                        dump(item);
                        unindent();
                        new_line();
                    }
                }
            } break;

            case type::mapping:
            {
                const auto& unordered_map = _node.as_mapping();
                assert(style == block_style);
                if (unordered_map.empty())
                {
                    write("{}");
                    break;
                }

                vector<mapping_type::const_iterator> ordered;
                ordered.reserve(unordered_map.size());
                for (auto iter = unordered_map.begin(); iter != unordered_map.end(); ++iter)
                    ordered.push_back(iter);
                std::sort(ordered.begin(), ordered.end(), [](auto lhs, auto rhs) { return lhs->first < rhs->first; });

                if (should_flow(_node))
                {
                    style = flow_style;
                    write('{');
                    for (const auto iter : ordered)
                    {
                        write(iter->first);
                        write(": ");
                        dump(iter->second);
                        write(", ");
                    }
                    output.pop_back();
                    output.back() = '}';
                    style = block_style;
                }
                else
                {
                    for (const auto iter : ordered)
                    {
                        write(iter->first);
                        write(": ");

                        const auto& item = iter->second;
                        if (item.type() == type::mapping && !should_flow(item)) // block map in block map, need indent
                        {
                            write_tag(item);
                            new_line();
                            indent();
                            dump(item);
                            unindent();
                        }
                        else if (item.type() == type::sequence && !should_flow(item)) // block seq in block map, no indent
                        {
                            write_tag(item);
                            new_line();
                            dump(item);
                        }
                        else
                            dump(item);
                        new_line();
                    }
                }
            } break;

            } // switch
        }
    };

    string dump(const node& _node)
    {
        dumper dumper;
        dumper.dump(_node);

        return dumper.output;
    }

}