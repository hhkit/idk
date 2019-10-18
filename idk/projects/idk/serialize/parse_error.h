#pragma once

namespace idk
{
    enum class parse_error : char
    {
        none = 0,
        invalid_argument,
        result_out_of_range,
        type_cannot_be_parsed,
        yaml_ill_formed,
        yaml_contains_tabs
    };
}