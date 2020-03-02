#pragma once
#include <vkn/BaseRenderPass.h>
#include <vkn/DrawSet.h>

namespace idk::vkn::renderpasses
{
	class DrawSetRenderPass : public BaseRenderPass
	{
	public:
		void Execute(Context_t&) final;
		virtual void Execute(Context_t&, BaseDrawSet& draw_set) = 0;
	private:
	};
	template<typename Pass, typename DrawSet>
	struct PassSetPair : BaseRenderPass
	{
		template<typename Arg, typename ... Args>
		PassSetPair(Arg&& arg, DrawSet&& ds, Args&&... args) :
			_render_pass{ std::forward<Arg>(arg), std::forward<Args>(args)... },
			_draw_set{ std::move(ds) }
		{
		}
		template<typename Arg, typename ... Args>
		PassSetPair(Arg&& arg, const DrawSet& ds, Args&&... args)
			:
			_render_pass{ std::forward<Arg>(arg),std::forward<Args>(args)... },
			_draw_set{ ds }
		{
		}
		void Execute(FrameGraphDetail::Context_t context)
		{
			_render_pass.name = name;
			Execute(context, _render_pass);
		}
		Pass& RenderPass()noexcept
		{
			return _render_pass;
		}
		operator Pass& ()noexcept
		{
			return RenderPass();
		}
	private:
		void Execute(Context_t context, DrawSetRenderPass& rp)
		{
			rp.Execute(context, _draw_set);
		}
		Pass _render_pass;
		DrawSet _draw_set;
	};
}