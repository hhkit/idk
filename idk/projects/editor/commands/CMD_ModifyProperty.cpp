#include "pch.h"
#include "CMD_ModifyProperty.h"

namespace idk
{

    CMD_ModifyProperty::CMD_ModifyProperty(GenericHandle component, string_view property_path, reflect::dynamic old_value, reflect::dynamic new_value)
        : component{ component }, property_path{ property_path }, old_value{ old_value }, new_value{ new_value }
    {
    }

    // stolen from PrefabUtility
    static reflect::dynamic resolve_property_path(const reflect::dynamic& obj, string_view path)
    {
        size_t offset = 0;
        reflect::dynamic curr;

        while (offset < path.size())
        {
            auto end = path.find('/', offset);
            if (end == string::npos)
                end = path.size();
            const string_view token(path.data() + offset, end - offset);

            if (curr.valid())
            {
                if (curr.type.is_container())
                {
                    auto cont = curr.to_container();
                    if (cont.value_type.is_template<std::pair>())
                    {
                        auto key_type = cont.value_type.create().unpack()[0].type;
                        curr.swap(cont[*parse_text(string(token), key_type)]);
                    }
                    else
                        curr.swap(cont[*parse_text<size_t>(string(token))]);
                }
                else if (curr.type.is_template<std::variant>())
                    curr.swap(curr.get_variant_value().get_property(token).value);
                else
                    curr.swap(obj.get_property(token).value);
            }
            else
                curr.swap(obj.get_property(token).value);

            offset = end;
            ++offset;
        }

        return curr;
    }

    bool CMD_ModifyProperty::execute()
    {
        resolve_property_path(*component, property_path) = new_value;
        return true;
    }

    bool CMD_ModifyProperty::undo()
    {
        resolve_property_path(*component, property_path) = old_value;
        return true;
    }

}
