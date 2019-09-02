#include "stdafx.h"
#include "yaml.h"

namespace idk::yaml
{
	enum _mode : char { block_map, block_seq, flow_map, flow_seq, comment, unknown };
	struct parser_state
	{
		parser_state(string_view sv) : p{ sv.begin() }, end{ sv.end() } {}

		node root;
		vector<node*> stack{ &root };
		vector<_mode> mode_stack{ unknown };
		vector<int> block_indents;
		string token;
		bool new_block = true;
		string_view::iterator p;
		const string_view::iterator end;

		_mode& mode() { return mode_stack.back(); }

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
        if (p.mode() == flow_map || p.mode() == flow_seq)
        {
            ++p;
            return;
        }

		if (p.new_block && (p[1] == ' ' || p[1] == '\t' || p[1] == '\n' || p[1] == '\r'))
		{
			p.mode() = block_seq;
			p.mode_stack.push_back(unknown);
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
					p.mode_stack.pop_back();
					p.stack.pop_back();
				}
				else // same indentation, must be another list item
				{
                    if (*p == '-')
                    {
                        p.mode_stack.pop_back();
                        p.stack.pop_back();
                    }
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
			p.token += *p;
			++p;
		}
	}

	static void on_colon(parser_state& p)
	{
        if (p.mode() == flow_map && (p[1] == ' ' || p[1] == '\t' || p[1] == '\n' || p[1] == '\r'))
        {
            strip_trailing_ws(p.token);
            p.stack.push_back(&(*p.stack.back())[p.token]);
            p.token.clear();
            skipws_until_lf(++p);
            return;
        }
		else if (p.new_block && (p[1] == ' ' || p[1] == '\t' || p[1] == '\n' || p[1] == '\r'))
		{
			p.mode() = block_map;
			p.mode_stack.push_back(unknown);
            strip_trailing_ws(p.token);
			p.stack.push_back(&(*p.stack.back())[p.token]);
			p.token.clear();
			skipws_until_lf(++p);
			p.new_block = false;
			return;
		}

		if (!p.new_block || printable(p[1]))
		{
			p.token += *p;
			++p;
		}
	}

	static void on_curly_brace(parser_state& p)
	{
        if (p.token.size())
        {
            p.token += *p;
            return;
        }

        if (p.mode() == unknown)
            p.mode() = flow_map;
        else if (p.mode() == flow_map)
            p.mode_stack.push_back(flow_map);
        else if (p.mode() == flow_seq)
        {
            p.mode_stack.push_back(flow_map);
            p.stack.push_back(&p.stack.back()->emplace_back());
        }
        *p.stack.back() = node{ mapping_type{} };
	}

	static void on_square_brace(parser_state& p)
	{
        if (p.token.size())
        {
            p.token += *p;
            return;
        }

        if (p.mode() == unknown)
            p.mode() = flow_seq;
        else if (p.mode() == flow_map)
            p.mode_stack.push_back(flow_seq);
        else if (p.mode() == flow_seq)
        {
            p.mode_stack.push_back(flow_seq);
            p.stack.push_back(&p.stack.back()->emplace_back());
        }
        *p.stack.back() = node{ sequence_type{} };
	}

    static void on_comma(parser_state& p)
    {
        strip_trailing_ws(p.token);
        if (p.token.size())
        {
            if (p.mode() == flow_seq)
                p.stack.back()->emplace_back(p.token);
            else
            {
                *p.stack.back() = node{ p.token };
                p.stack.pop_back();
            }
            p.token.clear();
        }
        skipws_until_lf(++p);
    }

    static void on_flow_close(parser_state& p)
    {
        if ((p.mode() == flow_seq && *p == '}') ||
            (p.mode() == flow_map && *p == ']'))
        {
            p.token += *p;
            ++p;
            return;
        }

        strip_trailing_ws(p.token);
        if (p.token.size())
        {
            if (p.mode() == flow_seq)
                p.stack.back()->emplace_back(p.token);
            else
            {
                *p.stack.back() = node{ p.token };
                p.stack.pop_back();
            }
            p.token.clear();
        }
        p.stack.pop_back(); 
        p.mode_stack.pop_back(); 
        skipws_until_lf(++p);
    }

    static void on_exclamation_mark(parser_state& p)
    {
        if (p.token.size())
        {
            p.token += *p;
            return;
        }
        while (++p)
        {
            if (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r')
            {
                skipws_until_lf(p);
                break;
            }

            p.token += *p;
        }
    }

	static void on_lf(parser_state& p)
	{
        if (p.mode() == flow_map || p.mode() == flow_seq)
        {
            ++p;
            return;
        }

		strip_trailing_ws(p.token);

		int indent = handle_indent(p);
		if (!p)
			return;

		if (p.token.size())
		{
			*p.stack.back() = node{ p.token };
			p.stack.pop_back();
			p.token.clear();
            p.mode_stack.pop_back();
		}

		if (indent > p.block_indents.back())
		{
			p.block_indents.push_back(indent);
		}
		else if (indent < p.block_indents.back())
		{
			p.block_indents.pop_back();
			if (p.mode() == block_map)
				p.stack.pop_back();
            p.mode_stack.pop_back();

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
					p.mode_stack.pop_back();
				}
				else
					break;
			}
		}

		p.new_block = true;
	}

	node parse(string_view str)
	{
		parser_state p{ str };

		{
			int indent = handle_indent(p);
			p.block_indents.push_back(indent);
		}

		while (p && p.stack.size())
		{
            switch (*p)
            {
            case '\r': break;
            case '\n': on_lf(p); continue;

            case '-': on_hyphen(p); continue;
            case ':': on_colon(p); continue;
            case ',': on_comma(p); continue;
            case '{': on_curly_brace(p); break;
            case '[': on_square_brace(p); break;
            case '}':
            case ']': on_flow_close(p); continue;
            default: { if (printable(*p)) p.token += *p; } break;
            }

			++p;
		} // while

		if (p.token.size())
		{
			strip_trailing_ws(p.token);
			*p.stack.back() = node{ p.token };
		}

		return p.root;
	}

}