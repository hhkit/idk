#pragma once
#include <vkn/BaseRenderPass.h>
#include <vkn/FrameGraphBuilder.h>
#include <vkn/DrawSet.h>
#include <gfx/DefaultShaders.h>
#include <vkn/DrawSetRenderPass.h>
#include <gfx/PostProcessEffect.h>
#include <WindowsApplication.h>
namespace idk
{
	struct renderer_attributes;

	namespace vkn
	{
		class VulkanMesh;
		struct GraphicsState;
		struct RenderStateV2;
		class VknRenderTarget;

		struct FrameGraph;
	}
}
namespace idk::vkn::renderpasses
{

	struct PassUtil : BaseRenderPass
	{
		struct FullRenderData
		{
			const GraphicsState* gfx_state;
			RenderStateV2* rs_state;
			const GraphicsState& GetGfxState()const
			{
				return *gfx_state;
			}
			RenderStateV2& GetRenderState()const
			{
				return *rs_state;
			}
		};
		FullRenderData render_data;

		PassUtil() = default;
		PassUtil(FullRenderData rd) :render_data{ rd } {}
	};

	struct CopyDepthPass : BaseRenderPass
	{
		FrameGraphResource copied_depth;
		FrameGraphResource original_depth;
		uvec2 size;
		CopyDepthPass(FrameGraphBuilder& builder, uvec2 depth_size, FrameGraphResource depth);
		void Execute(FrameGraphDetail::Context_t context) override;
	};
	struct CopyColorPass : BaseRenderPass
	{
		string c_pass_name; 
		FrameGraphResource copied_color;
		FrameGraphResource original_color;
		uvec2 size;
		CopyColorPass(FrameGraphBuilder& builder, uvec2 color_size, FrameGraphResource color, vk::ImageLayout imageLayoutToConvert = vk::ImageLayout::eGeneral);
		CopyColorPass(FrameGraphBuilder& builder, uvec2 color_size, RscHandle<VknTexture> color,vk::ImageLayout imageLayoutToConvert = vk::ImageLayout::eGeneral);
		//void Init(FrameGraphBuilder& builder, uvec2 color_size, FrameGraphResource color, vk::ImageLayout il);
		void Execute(FrameGraphDetail::Context_t context) override;
	};
	struct GBufferPass : DrawSetRenderPass
	{
		FrameGraphResourceMutable gbuffer_rscs[5];
		FrameGraphResourceMutable depth_rsc;

		uvec2 rt_size;

		GBufferPass(FrameGraphBuilder& builder, uvec2 size, FrameGraphResource depth);
		void Execute(FrameGraphDetail::Context_t context, BaseDrawSet& draw_set) override;
	};

	struct FsqUtil
	{
		inline const static renderer_attributes fsq_requirements =
		{
			{
				{vtx::Attrib::Position,0},
				{vtx::Attrib::UV,1},
			}
		};
	};

	struct AccumPass : DrawSetRenderPass, FsqUtil
	{
		FrameGraphResourceMutable accum_rsc;
		FrameGraphResourceReadOnly depth_rsc;
		GBufferPass& gbuffer_pass;

		FragmentShaders deferred_post;

		uvec2 rt_size;

		AccumPass(FrameGraphBuilder& builder, GBufferPass& gbuffers);
		void Execute(FrameGraphDetail::Context_t context, BaseDrawSet& draw_set) override;
	};

	struct CombinePass : BaseRenderPass, FsqUtil
	{
		FrameGraphResourceMutable out_color;
		FrameGraphResourceMutable out_hdr;
		FrameGraphResourceMutable out_depth;

		RscHandle<ShaderProgram> combine_shader;

		VknTextureView color_correction_lut;

		PostProcessEffect ppe;

		CombinePass(FrameGraphBuilder& builder, rect viewport, FrameGraphResource in_color_tex, FrameGraphResource in_depth_tex,FrameGraphResource out_color_tex,  FrameGraphResource out_depth_tex, FrameGraphResource view_pass_color);
		void Execute(FrameGraphDetail::Context_t context) override;
		rect _viewport;
	};
	struct CombinePassSpec : BaseRenderPass, FsqUtil
	{
		FrameGraphResourceMutable out_color;
		FrameGraphResourceMutable out_hdr;
		FrameGraphResourceMutable out_depth;

		RscHandle<ShaderProgram> combine_shader;

		VknTextureView color_correction_lut;

		CombinePassSpec(FrameGraphBuilder& builder, rect viewport, FrameGraphResource in_color_tex, FrameGraphResource in_depth_tex, FrameGraphResource out_color_tex, FrameGraphResource out_depth_tex);
		void Execute(FrameGraphDetail::Context_t context) override;
		rect _viewport;
	};
	struct HdrPass : BaseRenderPass, FsqUtil
	{
		FrameGraphResourceMutable hdr_rsc;
		FrameGraphResourceMutable hdr_depth_rsc;
		AccumPass& accum_def, & accum_spec;
	
		FrameGraphResourceMutable accum_att_def, depth_att_def;
		FrameGraphResourceMutable accum_att_spec, depth_att_spec;
	
		RscHandle<ShaderProgram> hdr_shader;
	
		HdrPass(FrameGraphBuilder& builder, AccumPass& accum_def_, AccumPass& accum_spec_, rect viewport, FrameGraphResource color_tex, FrameGraphResource depth_tex);
		void Execute(FrameGraphDetail::Context_t context) override;
		rect _viewport;
	};

	struct UnlitPass: DrawSetRenderPass
	{
		FrameGraphResourceMutable color_rsc;
		FrameGraphResourceMutable depth_rsc;

		UnlitPass(FrameGraphBuilder& builder, FrameGraphResource color_tex, FrameGraphResource depth_rsc);
		void Execute(Context_t context, BaseDrawSet& draw_set) override;
		rect _viewport;
	};


	struct CubeClearPass : DrawSetRenderPass, FsqUtil
	{

		renderer_attributes req = { {
			std::make_pair(vtx::Attrib::Position, 0),
			std::make_pair(vtx::Attrib::Normal, 1),
			std::make_pair(vtx::Attrib::UV, 2) }
		};
		FrameGraphResource render_target;
		FrameGraphResource depth;
		uvec2 rt_size;
		CubeClearPass(FrameGraphBuilder& builder, RscHandle<RenderTarget> rt, bool col_dont_care,std::optional<color>clear_color, std::optional<float> clear_depth);
		void Execute(Context_t context, BaseDrawSet& draw_set)override;
	};

	struct DeferredRendering
	{
		//returns color and depth
		static std::pair<FrameGraphResource, FrameGraphResource> MakePass(FrameGraph& graph, RscHandle<VknRenderTarget> rt, const GraphicsState& gfx_state, RenderStateV2& rs);
	};

}