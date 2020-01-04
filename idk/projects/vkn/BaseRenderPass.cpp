#include "pch.h"
#include "BaseRenderPass.h"

//Run to Acquire resources and transition nodes.
namespace idk::vkn
{

	void BaseRenderPass::PreExecute(const FrameGraphNode& node, Context_t context)
	{
		BeginRenderPass(context);
		BindFrameBuffer(context);
	}

	void BaseRenderPass::PostExecute(const FrameGraphNode& node, Context_t context)
	{
		EndRenderPass(context);
	}

}