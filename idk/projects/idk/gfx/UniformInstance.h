#pragma once
#include <idk.h>
namespace idk
{
	using UniformInstanceValue = variant<std::monostate, float, int, vec2, vec3, vec4, RscHandle<Texture>>;
    struct UniformInstance
    {
        string name; // in shader
        UniformInstanceValue value;
    };
}