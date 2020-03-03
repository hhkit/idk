#pragma once
#include "RenderContext.h"
namespace idk::vkn
{
	struct FrameGraphNode;
	struct FrameGraphResource;
	

	struct BaseRenderPass
	{
		//BaseRenderPass(FrameGraphBuilder&,...); //<-- First parameter required, will be forwarded from add command.
		using Context_t = FrameGraphDetail::Context_t;
		VknRenderPass  render_pass;
		Framebuffer frame_buffer;
		uvec2       fb_size;

		string name;

		//Run to Begin the rendering context (renderpass/framebuffer)
		void PreExecute(const FrameGraphNode& node, Context_t context);
		virtual void Execute(FrameGraphDetail::Context_t context) = 0;
		void PostExecute(const FrameGraphNode& node, Context_t context);
		////Executed just before the processed context stuff is applied to the command buffer
		//virtual void PreCommandBuffer(vk::CommandBuffer command_buffer);
		////Executed immediately after the processed context stuff is applied to the command buffer
		//virtual void PostCommandBuffer(vk::CommandBuffer command_buffer);
		virtual ~BaseRenderPass() = default;

	private:
		vector<VknTextureView> _input_attachments;
		vector<string_view> _input_attachment_names;
		void BeginRenderPass(Context_t);
		void EndRenderPass(Context_t);
		void BindFrameBuffer(Context_t);
	};
}