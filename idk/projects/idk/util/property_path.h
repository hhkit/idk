#pragma once

#include <reflect/reflect.h>

namespace idk
{

    reflect::dynamic resolve_property_path(const reflect::dynamic& obj, string_view path);

}