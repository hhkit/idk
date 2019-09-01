#include "stdafx.h"
#include "yaml.h"

namespace idk::yaml
{
    enum _mode : char { block_map, block_seq, flow_map, flow_seq, comment };

    static bool printable(int c)
    {
        return c >= (unsigned char)' ' && c <= (unsigned char)'~';
    }
    static bool skipws_until_lf(string_view::iterator& p, string_view::iterator end)
    {
        while (p != end)
        {
            if (*p == '\n' || (p[0] == '\r' && p[1] == '\n'))
                return true;
            else if (*p == ' ' || *p == '\t')
                ++p;
            else if (printable(*p))
                return false;
        }
        return true;
    }
    static int handle_indent(string_view::iterator& p, string_view::iterator end)
    {
        int indent = 0;
        while (p != end)
        {
            if (*p == ' ')
                ++indent;
            else if (*p == '\n' || (p[0] == '\r' && p[1] == '\n'))
                indent = 0;
            else if (printable(*p))
                break;
            ++p;
        }
        return indent;
    }
    static int handle_indent(string_view::iterator& p, string_view::iterator end, bool& hit_lf)
    {
        int indent = 0;
        hit_lf = false;
        while (p != end)
        {
            if (*p == ' ')
                ++indent;
            else if (*p == '\n' || (p[0] == '\r' && p[1] == '\n'))
            {
                indent = 0;
                hit_lf = true;
            }
            else if (printable(*p))
                break;
            ++p;
        }
        return indent;
    }
    static void strip_trailing_ws(string& str)
    {
        while (str.size() && isspace(str.back()))
            str.pop_back();
    }

    node parse(string_view str)
    {
        node root;
        vector<node*> stack{ &root };
        vector<int> block_indents;
        string curr_str;
        _mode curr_mode = block_map;
        bool new_block = true;

        auto p = str.begin();
        auto end = str.end();

        {
            int indent = handle_indent(p, end);
            block_indents.push_back(indent);
        }

        while(p != end && stack.size())
        {
            char c = *p;

            if (c == '-' || c == ':')
            {
                if(new_block && (p[1] == ' ' || p[1] == '\t' || p[1] == '\n' || p[1] == '\r'))
                {
                    if (c == '-')
                    {
                        curr_mode = block_seq;
                        stack.push_back(&stack.back()->emplace_back());
                        bool hit_lf;
                        int indent = handle_indent(++p, end, hit_lf);
                        if (hit_lf)
                        {
                            if (indent > block_indents.back())
                                block_indents.push_back(indent);
                            else if (indent < block_indents.back())
                            {
                                block_indents.pop_back();
                                stack.pop_back();
                            }
                            else // same indentation, must be another list item
                            {
                                if(*p != '-')
                                    stack.pop_back();
                            }
                        }
                        else // add indentation for list, ie - x is 2 indentation (for hyphen and space)
                        { 
                            block_indents.push_back(indent + 1 + block_indents.back());
                        }
                    }
                    else if (c == ':')
                    {
                        curr_mode = block_map;
                        stack.push_back(&(*stack.back())[curr_str]);
                        curr_str.clear();
                        skipws_until_lf(++p, end);
                        new_block = false;
                    }
                    continue;
                }
                
                if (!new_block || printable(p[1]))
                {
                    curr_str += c;
                }
            }

            else if (c == '\n' || (c == '\r' && p[1] == '\n'))
            {
                strip_trailing_ws(curr_str);

                int indent = handle_indent(p, end);
                if (p == end)
                    break;

                if (curr_str.size())
                {
                    *stack.back() = node{ curr_str };
                    stack.pop_back();
                    curr_str.clear();
                }

                if (curr_mode == block_map || curr_mode == block_seq)
                {
                    if (indent > block_indents.back())
                    {
                        block_indents.push_back(indent);
                    }
                    else if (indent < block_indents.back())
                    {
                        block_indents.pop_back();
                        stack.pop_back();

                        // keep going until match an old indent
                        while (block_indents.size())
                        {
                            if (indent > block_indents.back())
                            {
                                throw "sibling indent mismatch?";
                            }
                            else if (indent < block_indents.back())
                            {
                                stack.pop_back();
                                block_indents.pop_back();
                            }
                            else
                                break;
                        }
                    }

                    new_block = true;
                    continue;
                }
            }

            else if (printable(c))
            {
                curr_str += c;
            }

            ++p;

        } // while

        if(curr_str.size())
        {
            strip_trailing_ws(curr_str);
            *stack.back() = node{ curr_str };
        }

        return root;
    }



    type node::type()
    {
        return yaml::type(_value.index());
    }

    scalar_type& node::as_scalar()
    {
        return std::get<static_cast<int>(type::scalar)>(_value);
    }

    sequence_type& node::as_sequence()
    {
        return std::get<static_cast<int>(type::sequence)>(_value);
    }

    mapping_type& node::as_mapping()
    {
        return std::get<static_cast<int>(type::mapping)>(_value);
    }

    void node::push_back(const node& node)
    {
        if (type() == type::null)
            _value = sequence_type();
        as_sequence().push_back(node);
    }

    node& node::operator[](size_t index)
    {
        if (type() == type::null)
            _value = sequence_type();
        return as_sequence()[index];
    }

    node& node::operator[](const scalar_type& str)
    {
        if (type() == type::null)
            _value = mapping_type();
        return as_mapping()[str];
    }
}
