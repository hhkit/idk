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
		template<typename ... Args>
			PassSetPair(DrawSet&& ds, Args&&... args) :
			_render_pass{ std::forward<Args>(args)... },
			_draw_set{ std::move(ds) }
		{
		}
		template<typename ... Args>
		PassSetPair(const DrawSet& ds,Args&&... args) 
			: 
			_render_pass{std::forward<Args>(args)...},
			_draw_set{ds}
		{
		}
		void Execute(FrameGraphDetail::Context_t context)
		{
			Execute(context, _render_pass);
		}
		Pass& RenderPass()
		{
			return _render_pass;
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

	struct HdrPass : BaseRenderPass, FsqUtil
	{
		FrameGraphResourceMutable hdr_rsc;
		AccumPass& accum;

		FrameGraphResourceMutable accum_att, depth_att;

		RscHandle<ShaderProgram> hdr_shader;

		HdrPass(FrameGraphBuilder& builder, AccumPass& accum_, rect viewport, FrameGraphResource color_tex);
		void Execute(FrameGraphDetail::Context_t context) override;
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
		CubeClearPass(FrameGraphBuilder& builder, RscHandle<RenderTarget> rt,std::optional<color>clear_color, std::optional<float> clear_depth);
		void Execute(Context_t context, BaseDrawSet& draw_set)override;
	};

	struct DeferredRendering
	{
		//returns color and depth
		std::pair<FrameGraphResource, FrameGraphResource> MakePass(FrameGraph& graph, RscHandle<VknRenderTarget> rt, const GraphicsState& gfx_state, RenderStateV2& rs);
	};

}