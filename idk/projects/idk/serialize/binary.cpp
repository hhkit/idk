#include "stdafx.h"
#include "binary.h"

#include <reflect/reflect.h>

namespace idk
{

    template<>
    void detail::binary_serializer::operator<<(const reflect::dynamic& obj)
    {
        if (!obj.valid())
            return;
        else if (obj.type.is_enum_type())
        {
            *this << obj.to_enum_value().name();
            return;
        }
        else if (obj.type.count() == 0)
        {
#define SERIALIZE_CASE(TYPE) case reflect::typehash<TYPE>() : *this << obj.get<TYPE>(); return;
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
            default: break;
            }
#undef SERIALIZE_CASE

            if (obj.type.is_basic_serializable())
                *this << obj.to_string();
            else if (obj.type.is_container())
            {
                auto cont = obj.to_container();
                *this << cont.size();
                for (auto elem : cont)
                    *this << elem;
            }
            else if (obj.type.is_template<std::pair>())
            {
                auto pair = obj.unpack();
                *this << pair[0];
                *this << pair[1];
            }
            else if (obj.type.is_template<std::variant>())
            {
                auto held = obj.get_variant_value();
                output += held.type.name();
                output += '\0';
                *this << held;
            }
            else if (obj.type.is<reflect::dynamic>())
            {
                auto& held = obj.get<reflect::dynamic>();
                output += held.type.name();
                output += '\0';
                *this << held;
            }
            else
                throw "unhandled case?";

            return;
        }
        else if (obj.type.is_basic_serializable())
        {
            *this << obj.to_string();
            return;
        }

        obj.visit([&](auto&&, auto&& value, int)
        {
            *this << value;
            return false;
        });
    }

    template<>
    void detail::binary_parser::operator>>(reflect::dynamic& obj)
    {
        if (!obj.valid())
        {
            if (*begin == '\0')
            {
                ++begin;
                return;
            }

            // type delimited by null char
            string_view name = begin;
            begin += name.size() + 1;
            obj.swap(reflect::get_type(name).create());
        }
        
        if (obj.type.is_enum_type())
        {
            size_t sz;
            *this >> sz;
            string_view name{ begin, sz };
            begin += sz;

            obj = obj.to_enum_value().try_assign(name).value();
            return;
        }
        else if (obj.type.count() == 0)
        {
#define SERIALIZE_CASE(TYPE) case reflect::typehash<TYPE>() : *this >> obj.get<TYPE>(); return;
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
            default: break;
            }
#undef SERIALIZE_CASE

            if (obj.type.is_basic_serializable())
            {
                string str;
                *this >> str;
                obj = str;
            }
            else if (obj.type.is_container())
            {
                size_t sz;
                *this >> sz;

                auto cont = obj.to_container();
                for (size_t i = 0; i < sz; ++i)
                {
                    auto elem = cont.value_type.create();
                    *this >> elem;
                    cont.add(elem);
                }
            }
            else if (obj.type.is_template<std::pair>())
            {
                auto pair = obj.unpack();
                *this >> pair[0];
                *this >> pair[1];
            }
            else if (obj.type.is_template<std::variant>())
            {
                // variant type delimited by null char
                string_view name = begin;
                begin += name.size() + 1;

                auto dyn = reflect::get_type(name).create();
                *this >> dyn;
                obj = dyn;
            }
            else if (obj.type.is<reflect::dynamic>())
            {
                auto& held = obj.get<reflect::dynamic>();
                *this >> held;
            }
            else
                error = parse_error::type_cannot_be_parsed;

            return;
        }
        else if (obj.type.is_basic_serializable())
        {
            string str;
            *this >> str;
            obj = str;
            return;
        }

        obj.visit([&](auto&&, auto&& value, int)
        {
            *this >> value;
            return false;
        });

        if (begin > end)
            error = parse_error::binary_ill_formed;
        else
            obj.on_parse();
    }



    template<>
    string serialize_binary(const reflect::dynamic& obj)
    {
        detail::binary_serializer s;
        s << obj;
        return s.output;
    }

    template<>
    string serialize_binary(const Scene& scene)
    {
        return string();
    }

    template<>
    parse_error parse_binary(string_view sv, reflect::dynamic& obj)
    {
        detail::binary_parser p{ sv.data(), sv.data() + sv.size() };
        p >> obj;
        return p.error;
    }

    template<>
    parse_error parse_binary(string_view sv, Scene& scene)
    {
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