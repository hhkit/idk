#include "stdafx.h"

#include "property_path.h"
#include <reflect/reflect.inl>
#include <script/ManagedObj.inl>

namespace idk
{

    reflect::dynamic resolve_property_path(const reflect::dynamic& obj, string_view path)
    {
        size_t offset = 0;
        reflect::dynamic curr;

        if (!obj.valid())
            return curr;

        while (offset < path.size())
        {
            auto end = path.find('/', offset);
            if (end == string::npos)
                end = path.size();
            const string_view token(path.data() + offset, end - offset);

            if (curr.valid())
            {
                if (curr.type.is<mono::ManagedObject>())
                {
                    string_view token2 = token;
                    reflect::dynamic ret;
                    curr.get<mono::ManagedObject>().Visit([&](auto&& key, auto&& arg, int depth_change)
                    {
                        if (serialize_text(key) == token2)
                        {
                            if (end == path.size())
                            {
                                ret.swap(reflect::dynamic{ arg }.copy());
                                return false;
                            }

                            end = path.find('/', offset);
                            if (end == string::npos)
                                end = path.size();
                            token2 = string_view(path.data() + offset, end - offset);
                            return true;
                        }
                        return false;
                    });
                    return ret;
                }

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

    void assign_property_path(const reflect::dynamic& obj, string_view path, reflect::dynamic value)
    {
        if (!value.valid() || !obj.valid())
            return;

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
                {
                    string_view token2 = token;
                    curr.get<mono::ManagedObject>().Visit([&](auto&& key, auto&& arg, int depth_change)
                    {
                        if (serialize_text(key) == token2)
                        {
                            if (end == path.size())
                            {
                                reflect::dynamic{ arg } = value;
                                return false;
                            }

                            offset = end + 1;
                            end = path.find('/', offset);
                            if (end == string::npos)
                                end = path.size();
                            token2 = string_view(path.data() + offset, end - offset);
                            return true;
                        }
                        return false;
                    });

                    return;
                }

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

        if (curr.valid())
            curr = value;
    }

}