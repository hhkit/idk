#include "stdafx.h"
#include "yaml.h"

namespace idk::yaml
{
    enum _mode : char { block_map, block_seq, flow_map, flow_seq, comment };
	struct parser_state
	{
		parser_state(string_view sv) : p{ sv.begin() }, end{ sv.end() } {}

		node root;
		vector<node*> stack{ &root };
		vector<int> block_indents;
		string str;
		_mode mode = block_map;
		bool new_block = true;
		string_view::iterator p;
		const string_view::iterator end;

		parser_state& operator++() { ++p; return *this; } // prefix
		char operator[](size_t i) const { return p[i]; } // subscript
		char operator*() const { return *p; } // subscript
		explicit operator bool() const { return p != end; }
	};

    static bool printable(int c)
    {
        return c >= (unsigned char)' ' && c <= (unsigned char)'~';
    }
    static bool skipws_until_lf(parser_state& p)
    {
        while (p)
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
    static int handle_indent(parser_state& p)
    {
        int indent = 0;
		while (p)
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
    static int handle_indent(parser_state& p, bool& hit_lf)
    {
        int indent = 0;
        hit_lf = false;
        while (p)
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

	static void on_hyphen(parser_state& p)
	{
		if (p.new_block && (p[1] == ' ' || p[1] == '\t' || p[1] == '\n' || p[1] == '\r'))
		{
			p.mode = block_seq;
			p.stack.push_back(&p.stack.back()->emplace_back());
			bool hit_lf;
			int indent = handle_indent(++p, hit_lf);
			if (hit_lf)
			{
				if (indent > p.block_indents.back())
					p.block_indents.push_back(indent);
				else if (indent < p.block_indents.back())
				{
					p.block_indents.pop_back();
					p.stack.pop_back();
				}
				else // same indentation, must be another list item
				{
					if (*p == '-')
						p.stack.pop_back();
					else
						throw "expected '-'";
				}
			}
			else // add indentation for list, ie - x is 2 indentation (for hyphen and space)
			{
				p.block_indents.push_back(indent + 1 + p.block_indents.back());
			}
			return;
		}

		if (!p.new_block || printable(p[1]))
		{
			p.str += *p;
			++p;
		}
	}

	static void on_colon(parser_state& p)
	{
		if (p.new_block && (p[1] == ' ' || p[1] == '\t' || p[1] == '\n' || p[1] == '\r'))
		{
			p.mode = block_map;
			p.stack.push_back(&(*p.stack.back())[p.str]);
			p.str.clear();
			skipws_until_lf(++p);
			p.new_block = false;
			return;
		}

		if (!p.new_block || printable(p[1]))
		{
			p.str += *p;
			++p;
		}
	}

	static void on_endline(parser_state& p)
	{
		strip_trailing_ws(p.str);

		int indent = handle_indent(p);
		if (!p)
			return;

		if (p.str.size())
		{
			*p.stack.back() = node{ p.str };
			p.stack.pop_back();
			p.str.clear();
		}

		if (p.mode == block_map || p.mode == block_seq)
		{
			if (indent > p.block_indents.back())
			{
				p.block_indents.push_back(indent);
			}
			else if (indent < p.block_indents.back())
			{
				p.block_indents.pop_back();
				if (p.mode == block_map)
					p.stack.pop_back();

				// keep going until match an old indent
				while (p.block_indents.size())
				{
					if (indent > p.block_indents.back())
					{
						throw "sibling indent mismatch?";
					}
					else if (indent < p.block_indents.back())
					{
						p.stack.pop_back();
						p.block_indents.pop_back();
					}
					else
						break;
				}
			}

			p.new_block = true;
		}
	}

    node parse(string_view str)
    {
		parser_state p{ str };

        {
            int indent = handle_indent(p);
            p.block_indents.push_back(indent);
        }

        while(p && p.stack.size())
        {
			switch (*p)
			{
			case '-': on_hyphen(p); continue;
			case ':': on_colon(p); continue;
			case '\r': break;
			case '\n': on_endline(p); continue;
			default: { if (printable(*p)) p.str += *p; }
			}

            ++p;
        } // while

        if(p.str.size())
        {
            strip_trailing_ws(p.str);
            *p.stack.back() = node{ p.str };
        }

        return p.root;
    }



    type node::type() const
    {
        return yaml::type(_value.index());
    }

	bool node::null() const
	{
		return type() == type::null;
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
