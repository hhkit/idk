#pragma once

#include <idk.h>
#include <res/ResourceHandle.h>

namespace idk::shadergraph
{
	struct ValueType;
}
namespace idk::shadergraph::helpers
{
	string default_value(ValueType type);

    float parse_float(const string& val);
    string serialize_value(float vec);

	vec2 parse_vec2(const string& val);
	string serialize_value(vec2 vec);

	vec3 parse_vec3(const string& val);
	string serialize_value(const vec3& vec);

	vec4 parse_vec4(const string& val);
	string serialize_value(const vec4& vec);

    RscHandle<Texture> parse_sampler2d(const string& val);
    string serialize_value(RscHandle<Texture> tex);
}