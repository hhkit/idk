#pragma once

#include <idk.h>

namespace idk
{
    struct PropertyOverride
    {
        string component_name;
        string property_path;
        int component_nth = 0;
    };
}