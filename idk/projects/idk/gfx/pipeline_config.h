#pragma once
#include <idk.h>
#include <optional>
#include <gfx/buffer_desc.h>
namespace idk
{
	enum FillType
	{
		eFill
		, eLine
	};
	struct pipeline_config
	{
		string_view frag_shader{};
		string_view vert_shader{};
		std::optional<ivec2> screen_size{};
		vector<buffer_desc> buffer_descriptions;
		FillType fill_type = eFill;
	};
}
