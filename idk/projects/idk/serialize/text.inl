#pragma once

#include "text.h"
#include <ds/result.inl>
#include <charconv>

namespace idk
{

	template<typename T>
    string serialize_text(const T& obj)
	{
        if constexpr (std::is_same_v<std::decay_t<T>, bool>)
            return obj ? "1" : "0";
        else if constexpr (std::is_arithmetic_v<std::decay_t<T>>)
        {
            using DecayedT = std::decay_t<T>;
            constexpr size_t sz =
                std::is_same_v<DecayedT, float> ? 15 :
                std::is_same_v<DecayedT, double> ? 24 :
                std::numeric_limits<DecayedT>::digits10 + 1;

            char buf[sz];
            const std::to_chars_result res = std::to_chars(buf, buf + sz, obj);
            IDK_ASSERT(res.ec == std::errc{}); // should not have errors since sz should always fit T in base10.

            return string(buf, res.ptr);
        }
        else if constexpr (std::is_same_v<std::decay_t<T>, const char*>)
            return string(obj);
        else if constexpr (is_basic_serializable_v<T>)
            return string(obj);
        else if constexpr (std::is_enum_v<std::decay_t<T>>)
            return serialize_text(static_cast<std::underlying_type_t<std::decay_t<T>>>(obj));
		else
			return serialize_text(reflect::dynamic{ obj });
	}

	template<typename T>
    parse_error parse_text(string_view sv, T& obj)
	{
        if constexpr (std::is_same_v<std::decay_t<T>, bool>)
        {
            obj = sv != "0" && sv != "false";
            return parse_error::none;
        }
        else if constexpr (std::is_arithmetic_v<std::decay_t<T>>)
        {
            const std::from_chars_result res = std::from_chars(sv.data(), sv.data() + sv.size(), obj);
            if (res.ec == std::errc{})
                return parse_error::none;
            else if (res.ec == std::errc::invalid_argument)
                return parse_error::invalid_argument;
            else
                return parse_error::result_out_of_range;
        }
        else
        {
            if constexpr (is_macro_enum_v<T>)
                obj = std::decay_t<T>::from_string(sv);
            else if constexpr (is_basic_serializable_v<T>)
                obj = T(string{ sv });
            else
            {
                reflect::dynamic dyn{ obj };
                parse_text(sv, dyn);
            }
            return parse_error::none;
        }
	}

	template<typename T>
    monadic::result<T, parse_error> parse_text(string_view sv)
	{
		T obj;
        const auto res = parse_text(sv, obj);
        if (res == parse_error::none)
            return obj;
        else
            return res;
	}

}