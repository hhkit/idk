#pragma once
namespace idk::vkn
{
	using VknRenderPass = int;
	using VknFrameBuffer = int;
	struct FrameGraphNode;
	struct FrameGraphResource;

	namespace FrameGraphDetail
	{
		using Context_t = int;
	}

	struct BaseRenderPass
	{
		//BaseRenderPass(FrameGraphBuilder&,...); //<-- First parameter required, will be supplemented when created.
		using Context_t = FrameGraphDetail::Context_t;
		VknRenderPass  render_pass;
		VknFrameBuffer frame_buffer;

		//Run to Acquire resources and transition nodes.
		void PreExecute(const FrameGraphNode& node, Context_t context);
		virtual void Execute(FrameGraphDetail::Context_t context) = 0;
		void PostExecute(const FrameGraphNode& node, Context_t context);
		virtual ~BaseRenderPass() = default;

	private:
		void BeginRenderPass(Context_t);
		void EndRenderPass(Context_t);
		void BindFrameBuffer(Context_t);
	};
}