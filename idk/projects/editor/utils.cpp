#include "pch.h"
#include "utils.h"

namespace idk
{
    string format_name(string_view name)
    {
        if (name.empty())
            return "";

        if (name.front() == '<' && name.back() == '>')
        {
            name.remove_prefix(1);
            name.remove_suffix(1);
        }

        string str{ name };
        str[0] = (char)toupper(str[0]);
        for (int i = 0; i < str.size(); ++i)
        {
            if (str[i] == '_')
            {
                str[i] = ' ';
                if (i + 1 < str.size())
                    str[i + 1] = (char)toupper(str[i + 1]);
            }
        }
        for (int i = 1; i < str.size(); ++i)
        {
            if (str[i] >= 'A' && str[i] <= 'Z' && str[i - 1] >= 'a' && str[i - 1] <= 'z')
            {
                str.insert(i, 1, ' ');
            }
        }
        return str;
    }
}
