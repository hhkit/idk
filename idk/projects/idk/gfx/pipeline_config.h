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
			eSampler,
			eAttachment
		};
		enum UniformStage
		{
			eVertex  ,
			eFragment,
			eGeometry,
			eTessEval,
			eTessCtrl,
			eCompute ,
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
	enum class BlendOp
	{
		eAdd              ,// = VK_BLEND_OP_ADD,
		eSubtract         ,// = VK_BLEND_OP_SUBTRACT,
		eReverseSubtract  ,// = VK_BLEND_OP_REVERSE_SUBTRACT,
		eMin              ,// = VK_BLEND_OP_MIN,
		eMax              ,// = VK_BLEND_OP_MAX,
	};
	enum class BlendFactor
	{
		eZero ,// = VK_BLEND_FACTOR_ZERO,
		eOne ,// = VK_BLEND_FACTOR_ONE,
		eSrcColor ,// = VK_BLEND_FACTOR_SRC_COLOR,
		eOneMinusSrcColor ,// = VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR,
		eDstColor ,// = VK_BLEND_FACTOR_DST_COLOR,
		eOneMinusDstColor ,// = VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR,
		eSrcAlpha ,// = VK_BLEND_FACTOR_SRC_ALPHA,
		eOneMinusSrcAlpha ,// = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
		eDstAlpha ,// = VK_BLEND_FACTOR_DST_ALPHA,
		eOneMinusDstAlpha ,// = VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA,
		eConstantColor ,// = VK_BLEND_FACTOR_CONSTANT_COLOR,
		eOneMinusConstantColor ,// = VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR,
		eConstantAlpha ,// = VK_BLEND_FACTOR_CONSTANT_ALPHA,
		eOneMinusConstantAlpha ,// = VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA,
		eSrcAlphaSaturate ,// = VK_BLEND_FACTOR_SRC_ALPHA_SATURATE,
		eSrc1Color ,// = VK_BLEND_FACTOR_SRC1_COLOR,
		eOneMinusSrc1Color ,// = VK_BLEND_FACTOR_ONE_MINUS_SRC1_COLOR,
		eSrc1Alpha ,// = VK_BLEND_FACTOR_SRC1_ALPHA,
		eOneMinusSrc1Alpha ,// = VK_BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA
	};
	struct AttachmentBlendConfig
	{
		/*blendEnable         */bool        blend_enable           = false;
		/*srcColorBlendFactor */BlendFactor src_color_blend_factor = BlendFactor::eOne;
		/*dstColorBlendFactor */BlendFactor dst_color_blend_factor = BlendFactor::eZero;
		/*colorBlendOp        */BlendOp     color_blend_op         = BlendOp    ::eAdd;
		/*srcAlphaBlendFactor */BlendFactor src_alpha_blend_factor = BlendFactor::eOne;
		/*dstAlphaBlendFactor */BlendFactor dst_alpha_blend_factor = BlendFactor::eOne;
		/*alphaBlendOp        */BlendOp     alpha_blend_op         = BlendOp    ::eAdd;
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
		vector<AttachmentBlendConfig> attachment_configs = { AttachmentBlendConfig{} };
	};
}
