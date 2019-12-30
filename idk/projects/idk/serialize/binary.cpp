#include "stdafx.h"
#include "binary.inl"

#include <reflect/reflect.inl>

namespace idk
{
    template<bool IsBigEndian>
    static void _write(detail::binary_serializer<IsBigEndian>& s, const reflect::dynamic& obj)
    {
        if (!obj.valid())
            return;
        else if (obj.type.is_enum_type())
        {
            s << obj.to_enum_value().name();
            return;
        }
        else if (obj.type.count() == 0)
        {
#define SERIALIZE_CASE(TYPE) case reflect::typehash<TYPE>() : s << obj.get<TYPE>(); return;
            switch (obj.type.hash())
            {
            SERIALIZE_CASE(bool);
            SERIALIZE_CASE(char);
            SERIALIZE_CASE(uint8_t);
            SERIALIZE_CASE(short);
            SERIALIZE_CASE(uint16_t);
            SERIALIZE_CASE(int);
            SERIALIZE_CASE(uint32_t);
            SERIALIZE_CASE(int64_t);
            SERIALIZE_CASE(uint64_t);
            SERIALIZE_CASE(float);
            SERIALIZE_CASE(double);
            SERIALIZE_CASE(string);
            SERIALIZE_CASE(const char*);
            SERIALIZE_CASE(Guid);
            default: break;
            }
#undef SERIALIZE_CASE

            if (obj.type.is_basic_serializable())
                s << obj.to_string();
            else if (obj.type.is_container())
            {
                auto cont = obj.to_container();
                s << cont.size();

                if (cont.value_type.hash() == reflect::typehash<reflect::dynamic>()) // stores a dynamic?!
                {
                    for (auto elem : cont)
                    {
                        s.output += elem.type.name();
                        s.output += '\0';
                        s << elem;
                    }
                }
                else
                {
                    for (auto elem : cont)
                        s << elem;
                }
            }
            else if (obj.type.is_template<std::pair>())
            {
                auto pair = obj.unpack();
                s << pair[0];
                s << pair[1];
            }
            else if (obj.type.is_template<std::variant>())
            {
                auto held = obj.get_variant_value();
                s.output += held.type.name();
                s.output += '\0';
                s << held;
            }
            else if (obj.type.is<reflect::dynamic>())
            {
                auto& held = obj.get<reflect::dynamic>();
                s.output += held.type.name();
                s.output += '\0';
                s << held;
            }
            else
                throw "unhandled case?";

            return;
        }
        else if (obj.type.is_basic_serializable())
        {
            s << obj.to_string();
            return;
        }

        obj.visit([&](auto&&, auto&& value, int)
        {
            s << value;
            return false;
        });
    }

    template<>
    template<>
    void detail::binary_serializer<true>::operator<<(const reflect::dynamic& obj)
    {
        _write(*this, obj);
    }
    template<>
    template<>
    void detail::binary_serializer<false>::operator<<(const reflect::dynamic& obj)
    {
        _write(*this, obj);
    }



    template<bool IsBigEndian>
    static void _read(detail::binary_parser<IsBigEndian>& p, reflect::dynamic& obj)
    {
        if (!obj.valid())
        {
            if (*p.begin == '\0')
            {
                ++p.begin;
                return;
            }

            // type delimited by null char
            string_view name = p.begin;
            p.begin += name.size() + 1;
            obj.swap(reflect::get_type(name).create());
        }
        
        if (obj.type.is_enum_type())
        {
            size_t sz;
            p >> sz;
            string_view name{ p.begin, sz };
            p.begin += sz;

            obj = obj.to_enum_value().try_assign(name).value();
            return;
        }
        else if (obj.type.count() == 0)
        {
#define SERIALIZE_CASE(TYPE) case reflect::typehash<TYPE>() : p >> obj.get<TYPE>(); return;
            switch (obj.type.hash())
            {
            SERIALIZE_CASE(bool);
            SERIALIZE_CASE(char);
            SERIALIZE_CASE(uint8_t);
            SERIALIZE_CASE(short);
            SERIALIZE_CASE(uint16_t);
            SERIALIZE_CASE(int);
            SERIALIZE_CASE(uint32_t);
            SERIALIZE_CASE(int64_t);
            SERIALIZE_CASE(uint64_t);
            SERIALIZE_CASE(float);
            SERIALIZE_CASE(double);
            SERIALIZE_CASE(string);
            SERIALIZE_CASE(const char*);
            SERIALIZE_CASE(Guid);
            default: break;
            }
#undef SERIALIZE_CASE

            if (obj.type.is_basic_serializable())
            {
                string str;
                p >> str;
                obj = str;
            }
            else if (obj.type.is_container())
            {
                size_t sz;
                p >> sz;

                auto cont = obj.to_container();
                for (size_t i = 0; i < sz; ++i)
                {
                    auto elem = cont.value_type.create();
                    p >> elem;
                    cont.add(elem);
                }
            }
            else if (obj.type.is_template<std::pair>())
            {
                auto pair = obj.unpack();
                p >> pair[0];
                p >> pair[1];
            }
            else if (obj.type.is_template<std::variant>())
            {
                // variant type delimited by null char
                string_view name = p.begin;
                p.begin += name.size() + 1;

                auto dyn = reflect::get_type(name).create();
                p >> dyn;
                obj = dyn;
            }
            else if (obj.type.is<reflect::dynamic>())
            {
                auto& held = obj.get<reflect::dynamic>();
                p >> held;
            }
            else
                p.error = parse_error::type_cannot_be_parsed;

            return;
        }
        else if (obj.type.is_basic_serializable())
        {
            string str;
            p >> str;
            obj = str;
            return;
        }

        obj.visit([&](auto&&, auto&& value, int)
        {
            p >> value;
            return false;
        });

        if (p.begin > p.end)
            p.error = parse_error::binary_ill_formed;
        else
            obj.on_parse();
    }

    template<>
    template<>
    void detail::binary_parser<true>::operator>>(reflect::dynamic& obj)
    {
        _read(*this, obj);
    }
    template<>
    template<>
    void detail::binary_parser<false>::operator>>(reflect::dynamic& obj)
    {
        _read(*this, obj);
    }



    template<>
    string serialize_binary(const Scene& )
    {
		//TODO @MAL (Ignore this cause Ivan said don't serialize scene to binary)
        return string();
    }

    template<>
    parse_error parse_binary(string_view , Scene& )
    {
		//TODO @MAL (Ignore this cause Ivan said don't serialize scene to binary)
        return parse_error();
    }

    monadic::result<reflect::dynamic, parse_error> parse_binary(string_view sv, reflect::type type)
    {
        auto dyn = type.create();
        const auto err = parse_binary(sv, dyn);
        if (err != parse_error::none)
            return err;
        return dyn;
    }

}