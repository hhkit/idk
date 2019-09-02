#pragma once

#include "yaml.h"

namespace idk::yaml
{
    template<typename T, typename>
    node::node(T&& arg)
    {
        if constexpr (is_basic_serializable_v<T>)
        {
            if constexpr (std::is_arithmetic_v<T>)
                _value = std::to_string(arg);
            else
                _value = string(arg);
        }
        else if constexpr (is_container_v<T>)
        {
            if constexpr (is_sequential_container_v<T>)
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

    template<typename T>
    T node::get() const
    {
        if constexpr (is_basic_serializable_v<T>)
        {
            if (type() == type::null)
            {
                if constexpr (std::is_arithmetic_v<T>)
                    return 0;
                else
                    return T();
            }
            else if (type() == type::scalar)
                return parse_text<T>(as_scalar());
            throw "only works on scalars";
        }
        throw "cannot convert scalar to T";
    }

    template<typename... T>
    node& node::emplace_back(T&&... args)
    {
        if (type() == type::null)
            _value = sequence_type();
        return as_sequence().emplace_back(args...);
    }
}