#pragma once

#include <idk.h>

namespace idk::shadergraph
{
	struct ValueType;
}
namespace idk::shadergraph::helpers
{
	string default_value(ValueType type);

	vec2 parse_vec2(const string& val);
	string serialize_value(const vec2& vec);

	vec3 parse_vec3(const string& val);
	string serialize_value(const vec3& vec);

	vec4 parse_vec4(const string& val);
	string serialize_value(const vec4& vec);
}