#include "stdafx.h"
#include "ShaderGraph_helpers.h"
#include <gfx/ShaderGraph_data.h>
#include <serialize/text.inl>
#include <regex>
#include <ds/result.inl>

namespace idk::shadergraph::helpers
{

	string default_value(ValueType type)
	{
		switch (type)
		{
		case ValueType::FLOAT:      return "0";
		case ValueType::VEC2:       return "0,0";
		case ValueType::VEC3:       return "0,0,0";
		case ValueType::VEC4:       return "0,0,0,0";
		case ValueType::SAMPLER2D:  return string(Guid());
		default: throw;
		}
	}


    float parse_float(const string& val)
    {
        return parse_text<float>(val).value_or(0);
    }
    string serialize_value(float vec)
    {
        return serialize_text(vec);
    }


	vec2 parse_vec2(const string& val)
	{
        std::string _val = val;
		vec2 v;
		std::smatch matches;
		if (std::regex_match(_val, matches, std::regex("([\\d\\.\\-]+),([\\d\\.\\-]+)")))
		{
            v[0] = parse_text<float>(string_view{ &*matches[1].first, static_cast<size_t>(matches[1].length()) }).value_or(0);
            v[1] = parse_text<float>(string_view{ &*matches[2].first, static_cast<size_t>(matches[2].length()) }).value_or(0);
		}
		return v;
	}
	string serialize_value(vec2 vec)
	{
		return serialize_text(vec[0]) + ',' + serialize_text(vec[1]);
	}


	vec3 parse_vec3(const string& val)
	{
        std::string _val = val;
		vec3 v;
		std::smatch matches;
		if (std::regex_match(_val, matches, std::regex("([\\d\\.\\-]+),([\\d\\.\\-]+),([\\d\\.\\-]+)")))
		{
            v[0] = parse_text<float>(string_view{ &*matches[1].first, static_cast<size_t>(matches[1].length()) }).value_or(0);
            v[1] = parse_text<float>(string_view{ &*matches[2].first, static_cast<size_t>(matches[2].length()) }).value_or(0);
            v[2] = parse_text<float>(string_view{ &*matches[3].first, static_cast<size_t>(matches[3].length()) }).value_or(0);
		}
		return v;
	}
	string serialize_value(const vec3& vec)
	{
		return serialize_text(vec[0]) + ',' + serialize_text(vec[1]) + ',' + serialize_text(vec[2]);
	}


	vec4 parse_vec4(const string& val)
	{
        std::string _val = val;
		vec4 v;
		std::smatch matches;
		if (std::regex_match(_val, matches, std::regex("([\\d\\.\\-]+),([\\d\\.\\-]+),([\\d\\.\\-]+),([\\d\\.\\-]+)")))
		{
			v[0] = parse_text<float>(string_view{ &*matches[1].first, static_cast<size_t>(matches[1].length()) }).value_or(0);
            v[1] = parse_text<float>(string_view{ &*matches[2].first, static_cast<size_t>(matches[2].length()) }).value_or(0);
            v[2] = parse_text<float>(string_view{ &*matches[3].first, static_cast<size_t>(matches[3].length()) }).value_or(0);
            v[3] = parse_text<float>(string_view{ &*matches[4].first, static_cast<size_t>(matches[4].length()) }).value_or(0);
		}
		return v;
	}
	string serialize_value(const vec4& vec)
	{
		return serialize_text(vec[0]) + ',' + serialize_text(vec[1]) + ',' + serialize_text(vec[2]) + ',' + serialize_text(vec[3]);
	}


    RscHandle<Texture> parse_sampler2d(const string& val)
    {
        return RscHandle<Texture>(Guid(val));
    }
    string serialize_value(RscHandle<Texture> tex)
    {
        return string(tex.guid);
    }

}