#include "pch.h"
#include "BaseRenderPass.h"

#include <vkn/FrameGraphNode.h>
#include <vkn/FrameGraphResourceManager.h>

//Run to Acquire resources and transition nodes.
namespace idk::vkn
{
//

	void BaseRenderPass::PreExecute(const FrameGraphNode& node, Context_t context)
	{
		BeginRenderPass(context);
		BindFrameBuffer(context);
		auto output_atts = node.GetOutputAtt();
		auto span = node.GetInputSpan();
		for (auto& input : span)
		{
			_input_attachments.emplace_back(context.Resources().Get<VknTextureView>(input.id));
			_input_attachment_names.emplace_back(context.Resources().Name(input));
		}
		context.SetInputAttachments(_input_attachments);
		context.SetOutputAttachmentSize(output_atts.size());
		if(node.depth_stencil)
			context.SetClearDepthStencil(node.depth_stencil->second.clear_value);
		context.SkipRenderPass(node.skip_render_pass);
	}

	void BaseRenderPass::PostExecute(const FrameGraphNode& , Context_t context)
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
		//context.bundle = this->dbg;
		context.SetRenderPass(this->render_pass);
	}

	void BaseRenderPass::EndRenderPass(Context_t)
	{
	}

	void BaseRenderPass::BindFrameBuffer(Context_t context)
	{
		context.SetFrameBuffer(this->frame_buffer,this->fb_size);
	}

}