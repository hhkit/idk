#include "stdafx.h"
#include "ShaderGraph_helpers.h"
#include <regex>

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

	vec2 parse_vec2(const string& val)
	{
		vec2 v;
		std::smatch matches;
		if (std::regex_match(val, matches, std::regex("([\\d\\.\\-]+),([\\d\\.\\-]+)")))
		{
			v[0] = std::stof(matches[1]);
			v[1] = std::stof(matches[2]);
		}
		return v;
	}

	string serialize_value(const vec2& vec)
	{
		return std::to_string(vec[0]) + ',' + std::to_string(vec[1]);
	}

	vec3 parse_vec3(const string& val)
	{
		vec3 v;
		std::smatch matches;
		if (std::regex_match(val, matches, std::regex("([\\d\\.\\-]+),([\\d\\.\\-]+),([\\d\\.\\-]+)")))
		{
			v[0] = std::stof(matches[1]);
			v[1] = std::stof(matches[2]);
			v[2] = std::stof(matches[3]);
		}
		return v;
	}

	string serialize_value(const vec3& vec)
	{
		return std::to_string(vec[0]) + ',' + std::to_string(vec[1]) + ',' + std::to_string(vec[2]);
	}

	vec4 parse_vec4(const string& val)
	{
		vec4 v;
		std::smatch matches;
		if (std::regex_match(val, matches, std::regex("([\\d\\.\\-]+),([\\d\\.\\-]+),([\\d\\.\\-]+),([\\d\\.\\-]+)")))
		{
			v[0] = std::stof(matches[1]);
			v[1] = std::stof(matches[2]);
			v[2] = std::stof(matches[3]);
			v[3] = std::stof(matches[4]);
		}
		return v;
	}

	string serialize_value(const vec4& vec)
	{
		return std::to_string(vec[0]) + ',' + std::to_string(vec[1]) + ',' + std::to_string(vec[2]) + ',' + std::to_string(vec[3]);
	}

}