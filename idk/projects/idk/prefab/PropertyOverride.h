#pragma once

#include <idk.h>
#include <reflect/reflect.h>

namespace idk
{
    struct PropertyOverride
    {
        string component_name;
        string property_path;
        int component_nth = 0;
        reflect::dynamic value;
    };

    struct ComponentNth
    {
        string component_name;
        int component_nth = 0;
    };
}