#pragma once
#include <idk.h>
#include <gfx/FramebufferFactory.h>
#include <vulkan/vulkan.hpp>
namespace idk::vkn
{
	using index_t = size_t;

	struct RenderPassFbInfo
	{
		vector<std::pair<AttachmentInfo, index_t>> attachments;
		std::optional<std::pair<AttachmentInfo, index_t>> depth_attachment;
	};

	struct RenderPassInfo;
	struct SubPassConfig
	{
		struct Attachment
		{
			vk::ImageLayout use = vk::ImageLayout::eColorAttachmentOptimal;
		};
		using attachment_t =std::pair<index_t, Attachment>;
		vector< attachment_t > inputs;
		vector< attachment_t > output;
		std::optional<attachment_t> depth;


		struct Data
		{
			vector< vk::AttachmentReference>   attachments_ref;
			vector< vk::AttachmentReference>   input_ref;


			vk::AttachmentReference   depth_attachment_ref;
			//TODO: Figure out how to deal with DepthStencil since vulkan only accepts them together, they can't be separate.
			vk::AttachmentReference stencil_attachment_ref;
			vk::AttachmentReference* pdepth_ref = nullptr;

			vk::SubpassDescription subpass;

		};

		Data data;

		void SetDepthAttachment(index_t attachment_index, vk::ImageLayout use);

		void AddInputAttachment(index_t attachment_index, vk::ImageLayout use);
		void AddOutputAttachment(index_t attachment_index, vk::ImageLayout use);

		void BuildSubpass();

		vk::SubpassDescription GetSubpassDesc()const;

		std::pair<vk::AccessFlags, vk::PipelineStageFlags> GetDep();


		static void ProcessAttachments(uint32_t num,
			vector<attachment_t>& output,
			vector<vk::AttachmentReference  >& attachment_refs);

	};
	struct RenderPassInfo
	{
		vector<AttachmentInfo> attachments;
		vector<vk::AttachmentDescription> attachment_desc;
		vector<SubPassConfig> subpasses;

		vector<vk::SubpassDependency>  dependencies;
		vector<vk::SubpassDescription> sub;
		vector<vk::AttachmentDescription> attachment_descs;


		index_t RegisterAttachment(AttachmentInfo attachment, vk::ImageLayout in, vk::ImageLayout out);
		void SetAttachment(index_t index,AttachmentInfo attachment, vk::ImageLayout in, vk::ImageLayout out);


		uint32_t  RegisterSubpass(SubPassConfig config);


		void AddDependency(uint32_t src, uint32_t dest,
			vk::AccessFlags src_acc = {},
			vk::AccessFlags dst_acc = {},
			vk::PipelineStageFlags src_stg={},
			vk::PipelineStageFlags dst_stg={}
			);

		vk::RenderPassCreateInfo BuildRenderPass();
	private:
		static void AddAttachment(AttachmentInfo attachment,
			vector<vk::AttachmentDescription>& attachment_desc, vk::ImageLayout in, vk::ImageLayout out);
		static void SetAttachment(index_t index,AttachmentInfo attachment,
			vector<vk::AttachmentDescription>& attachment_desc, vk::ImageLayout in, vk::ImageLayout out);
	};

	struct RpAttachmentInfo
	{
		AttachmentInfo attachment;
		vk::ImageLayout in;
		vk::ImageLayout out;
	};

}