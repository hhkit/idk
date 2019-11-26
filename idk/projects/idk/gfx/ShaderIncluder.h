#pragma once
#include <idk.h>

#include <core/Core.h>
#include <gfx/ShaderSnippet.h>
namespace idk
{

	struct IncludeEntry
	{
		size_t start, len;
		string_view include_name;
	};
	
	IncludeEntry FindNextIncludes(string_view& str_view, size_t& offset) noexcept;
	
	string_view GetIncluded(string_view mounted_dir);

	string ProcessIncludes(string_view glsl_code);
}