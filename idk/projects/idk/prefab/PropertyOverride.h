#pragma once

#include <idk.h>

namespace idk
{
    struct PropertyOverride
    {
		int object_index{};
        string component_name;
        string property_path;
    };
}