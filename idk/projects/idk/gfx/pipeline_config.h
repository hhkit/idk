#pragma once
#include <idk.h>
#include <optional>
#include <gfx/buffer_desc.h>
#include <gfx/ShaderProgram.h>
#include <gfx/BasicRenderPasses.h>
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
		enum UniformType
		{
			eBuffer,
			eSampler
		};
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
			UniformType type = eBuffer;
		};
		std::vector<bindings_t> bindings;
	};
	enum class CullFace
	{
		eNone = 0,
		eFront = 1 << 0,
		eBack  = 1 << 1,
	};
	struct pipeline_config
	{
		RscHandle<ShaderProgram> frag_shader{};
		RscHandle<ShaderProgram> vert_shader{};
		std::optional<ivec2> viewport_offset{};
		std::optional<ivec2> viewport_size{};
		vector<buffer_desc> buffer_descriptions;
		FillType fill_type = eFill;
		PrimitiveTopology prim_top = eTriangleList;
		BasicRenderPasses render_pass_type = {};
		//hash_table<uint32_t,uniform_layout_t> uniform_layouts; //Encapsulated in shader program.
		bool restart_on_special_idx = false; //Set to true to allow strips to be restarted with special indices 0xFFFF or 0xFFFFFFFF
		uint32_t cull_face = static_cast<uint32_t>(CullFace::eBack);
		bool depth_test  =true;
		bool depth_write = true;
		bool stencil_test =true;
	};
}
