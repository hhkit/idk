#pragma once
#include <vkn/renderpasses/DeferredPasses.h>
#include <gfx/PostProcessEffect.h>

namespace idk::vkn::renderpasses
{
	struct BloomPass : BaseRenderPass, FsqUtil
	{
		FrameGraphResourceMutable bloom_rsc;
		FrameGraphResourceMutable depth_rsc;
		//FrameGraphResourceMutable brightness_read_only;
		//FrameGraphResourceMutable bloom_depth_rsc;

		VknTextureView bright_texture;
		VknTextureView color_correction_lut;

		RscHandle<ShaderProgram> bloom_shader;

		PostProcessEffect ppe;

		BloomPass(FrameGraphBuilder& builder, FrameGraphResource out_color, FrameGraphResource color, FrameGraphResource depth, FrameGraphResource hdr, FrameGraphResource gViewPos, rect viewport);
		void Execute(FrameGraphDetail::Context_t context) override;
		rect _viewport;
	};

	struct BloomPassH : BaseRenderPass, FsqUtil
	{
		FrameGraphResourceMutable bloom_rsc;
		FrameGraphResourceMutable brightness_read_only;
		//FrameGraphResourceMutable bloom_depth_rsc;

		VknTextureView bright_texture;

		RscHandle<ShaderProgram> bloom_shader;

		PostProcessEffect ppe;

		BloomPassH(FrameGraphBuilder& builder, FrameGraphResource out_color, FrameGraphResource hdr, rect viewport);
		void Execute(FrameGraphDetail::Context_t context) override;
		rect _viewport;
	};

	struct BloomPassW : BaseRenderPass, FsqUtil
	{
		FrameGraphResourceMutable bloom_rsc;
		FrameGraphResourceMutable brightness_read_only;
		//FrameGraphResourceMutable bloom_depth_rsc;

		VknTextureView bright_texture;

		RscHandle<ShaderProgram> bloom_shader;

		PostProcessEffect ppe;

		BloomPassW(FrameGraphBuilder& builder, FrameGraphResource out_color, FrameGraphResource hdr, rect viewport);
		void Execute(FrameGraphDetail::Context_t context) override;
		rect _viewport;
	};
}