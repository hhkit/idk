#include "pch.h"
#include "CMD_ModifyProperty.h"
#include <script/ManagedObj.inl>
#include <script/MonoBehavior.h>
#include <serialize/text.inl>
#include <ds/result.inl>
#include <res/ResourceHandle.inl>

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
				if (curr.type.is<mono::ManagedObject>())
					return curr;
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
                    curr.swap(curr.get_property(token).value);
            }
            else
                curr.swap(obj.get_property(token).value);

            offset = end;
            ++offset;
        }

        return curr;
    }

	CMD_ModifyProperty::CMD_ModifyProperty(reflect::dynamic object, reflect::dynamic old_value)
        : object{ object }, new_value{object.copy()}, old_value{old_value}
	{
	}

	bool CMD_ModifyProperty::execute()
    {
        if (component.id)
        {
            if (component.is_type<mono::Behavior>())
                handle_cast<mono::Behavior>(component)->GetObject().Assign(string_view(property_path), new_value);
            else
                resolve_property_path(*component, property_path) = new_value;
        }
        else
        {
            object = new_value;
        }
        return true;
    }

    bool CMD_ModifyProperty::undo()
    {
        if (component.id)
        {
            if (component.is_type<mono::Behavior>())
                handle_cast<mono::Behavior>(component)->GetObject().Assign(string_view(property_path), old_value);
            else
                resolve_property_path(*component, property_path) = old_value;
        }
        else
            object = old_value;
        return true;
    }

}
