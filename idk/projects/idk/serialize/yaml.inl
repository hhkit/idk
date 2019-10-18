#pragma once

#include "yaml.h"
#include <serialize/text.h>

namespace idk::yaml
{
    static void resolve_scalar(scalar_type& scalar)
    {
        if (scalar.size() < 2)
            return;

        if (scalar.front() == '"' && scalar.back() == '"')
        {
            // remove double quotes
            scalar.pop_back();
            scalar.erase(scalar.begin());

            // handle escape sequences
            for (size_t i = 0; i + 1 < scalar.size(); ++i)
            {
                if (scalar[i] != '\\')
                    continue;

                switch (scalar[i + 1])
                {
                case '\\': scalar.replace(i, 2, 1, '\\'); break;
                case '"': scalar.replace(i, 2, 1, '\"');  break;
                case '0': scalar.replace(i, 2, 1, '\0');  break;
                case 't': scalar.replace(i, 2, 1, '\t');  break;
                case 'n': scalar.replace(i, 2, 1, '\n');  break;
                case 'r': scalar.replace(i, 2, 1, '\r');  break;
                default: break;
                }
            }
        }
        else if (scalar.front() == '\'' && scalar.back() == '\'')
        {
            // remove single quotes... don't handle escapes for single quoted scalars! except for '
            scalar.pop_back();
            scalar.erase(scalar.begin());

            for (size_t i = 0; i + 1 < scalar.size(); ++i)
            {
                if (scalar[i] == '\'' && scalar[i + 1] == '\'')
                    scalar.replace(i, 2, 1, '\'');
            }
        }
    }

    template<typename T, typename>
    node::node(T&& arg)
    {
        if constexpr (is_basic_serializable_v<T>)
        {
            _value = serialize_text(arg);
            resolve_scalar(as_scalar());
        }
        else if constexpr (is_container_v<T>)
        {
            if constexpr (std::is_same_v<sequence_type, std::decay_t<T>>)
                _value.emplace<sequence_type>(std::forward<T>(arg));
            else if constexpr (std::is_same_v<mapping_type, std::decay_t<T>>)
                _value.emplace<mapping_type>(std::forward<T>(arg));
            else if constexpr (is_sequential_container_v<T>)
            {
                auto& container = _value.emplace<sequence_type>();
                for (auto& elem : arg)
                    container.emplace_back(elem);
            }
            else // associative
            {
                auto& container = _value.emplace<mapping_type>();
                for (auto& elem : arg)
                    container.emplace(elem);
            }
        }
    }

    // get scalar as type T
    template<typename T, typename>
    decltype(auto) node::get() const
    {
        if (type() == type::null)
        {
            if constexpr (std::is_arithmetic_v<std::decay_t<T>>)
                return static_cast<T>(0);
            else
                return T();
        }
        else if (type() == type::scalar)
        {
            auto res = parse_text<T>(as_scalar());
            if (res)
                return static_cast<T>(*res);
            else
                return static_cast<T>(0);
        }

        throw "only works on scalars";
    }

    // returns const& to scalar
    template<>
    decltype(auto) node::get<scalar_type>() const
    {
        return as_scalar();
    }

    template<typename... Ts>
    node& node::emplace_back(Ts&&... args)
    {
        if (type() == type::null)
            _value = sequence_type();
        return as_sequence().emplace_back(std::forward<Ts>(args)...);
    }

    template<typename... Ts>
    std::pair<mapping_type::iterator, bool> node::emplace(Ts&&... args)
    {
        if (type() == type::null)
            _value = mapping_type();
        return as_mapping().emplace(std::forward<Ts>(args)...);
    }

    template<typename T, typename>
    node& node::operator=(T&& val)
    {
        node other(val);
        std::swap(_value, other._value);
        return *this;
    }
}