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
	enum PrimitiveTopology
	{
		eTriangleList,
		eTriangleStrip,
		eLineStrip,
		ePatchList,
		ePointList,
		eLineList ,
	};

	struct uniform_layout_t
	{
		enum UniformStage
		{
			eVertex  ,
			eFragment,
		};
		struct bindings_t
		{
			uint32_t binding;               //Can probably force it to be 1 binding
			uint32_t descriptor_count;      //
			hash_set<UniformStage> stages;  //Can probably force it to be both?
		};
		std::vector<bindings_t> bindings;
	};

	struct pipeline_config
	{
		string_view frag_shader{};
		string_view vert_shader{};
		std::optional<ivec2> screen_size{};
		vector<buffer_desc> buffer_descriptions;
		FillType fill_type = eFill;
		PrimitiveTopology prim_top = eTriangleList;
		hash_table<uint32_t,uniform_layout_t> uniform_layouts;
		bool restart_on_special_idx = false; //Set to true to allow strips to be restarted with special indices 0xFFFF or 0xFFFFFFFF
	};
}
