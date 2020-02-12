#pragma once
#include <vkn/BaseRenderPass.h>
#include <vkn/FrameGraphBuilder.h>
#include <vkn/DrawSet.h>
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
	class DrawSetRenderPass : public BaseRenderPass
	{
	public:
		void Execute(Context_t&) final;
		virtual void Execute(Context_t&, BaseDrawSet& draw_set)=0;
	private:
	};

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
	template<typename Pass, typename DrawSet>
	struct PassSetPair : BaseRenderPass
	{
		template<typename Arg,
			typename = std::enable_if_t<
				!std::is_same_v<
				PassSetPair,
				std::decay_t<Arg>
				>
			>,
			typename ... Args>
			PassSetPair(Arg&& arg, DrawSet&& ds, Args&&... args) :
			_render_pass{ std::forward<Arg>(arg),std::forward<Args>(args)... },
			_draw_set{ std::move(ds) }
		{
		}
		template<typename Arg, 
			typename = std::enable_if_t<
				!std::is_same_v<
					PassSetPair,
					std::decay_t<Arg>
				>
			>,
			typename ... Args>
		PassSetPair(Arg&& arg, const DrawSet& ds,Args&&... args) 
			: 
			_render_pass{std::forward<Arg>(arg),std::forward<Args>(args)...},
			_draw_set{ds}
		{
		}
		void Execute(FrameGraphDetail::Context_t context)
		{
			Execute(context, _render_pass);
		}
	private:
		void Execute(Context_t context, DrawSetRenderPass& rp)
		{
			rp.Execute(context, _draw_set);
		}
		Pass _render_pass;
		DrawSet _draw_set;
	};

	struct GBufferPass : DrawSetRenderPass
	{
		FrameGraphResourceMutable gbuffer_rscs[6];
		FrameGraphResourceMutable depth_rsc;
		GBufferPass(FrameGraphBuilder& builder, RscHandle<VknRenderTarget> rt, bool clear_depth);
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

		AccumPass(FrameGraphBuilder& builder, GBufferPass& gbuffers);
		void Execute(FrameGraphDetail::Context_t context, BaseDrawSet& draw_set) override;
	};

	struct HdrPass : PassUtil, FsqUtil
	{
		FrameGraphResourceMutable hdr_rsc;
		AccumPass& accum;

		FrameGraphResourceMutable accum_att, depth_att;

		RscHandle<ShaderProgram> hdr_shader;

		HdrPass(FrameGraphBuilder& builder, AccumPass& accum_, RscHandle<VknRenderTarget>, FullRenderData& rd);
		void Execute(FrameGraphDetail::Context_t context) override;
	};
	struct CubeClearPass : PassUtil, FsqUtil
	{

		renderer_attributes req = { {
			std::make_pair(vtx::Attrib::Position, 0),
			std::make_pair(vtx::Attrib::Normal, 1),
			std::make_pair(vtx::Attrib::UV, 2) }
		};
		FrameGraphResource render_target;
		CubeClearPass(FrameGraphBuilder& builder, FullRenderData& frd);
		void Execute(Context_t context)override;
	};

	struct ClearCombine : PassUtil, FsqUtil
	{
		static RscHandle<ShaderProgram> clear_merge;
		ClearCombine(FrameGraphBuilder& builder, RscHandle<VknRenderTarget>, FrameGraphResource clear_color_buffer, FrameGraphResource scene_color, FrameGraphResource scene_depth, FullRenderData& frd);
		void Execute(Context_t context)override;
	};
	RscHandle<ShaderProgram> ClearCombine::clear_merge = {};

	struct DeferredRendering
	{
		//returns color and depth
		std::pair<FrameGraphResource, FrameGraphResource> MakePass(FrameGraph& graph, RscHandle<VknRenderTarget> rt, const GraphicsState& gfx_state, RenderStateV2& rs);
	};

}