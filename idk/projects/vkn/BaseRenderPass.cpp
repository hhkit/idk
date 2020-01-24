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
	/*
	void BaseRenderPass::PreCommandBuffer(vk::CommandBuffer )
	{
	}

	void BaseRenderPass::PostCommandBuffer(vk::CommandBuffer )
	{
	}
	*/
	void BaseRenderPass::BeginRenderPass(Context_t context)
	{
		context.SetRenderPass(this->render_pass);
	}

	void BaseRenderPass::EndRenderPass(Context_t)
	{
	}

	void BaseRenderPass::BindFrameBuffer(Context_t context)
	{
		context.SetFrameBuffer(this->frame_buffer);
	}

}