#include "pch.h"
#include "RenderPassInfo.h"
#include <vkn/BufferHelpers.h>
#include <vkn/VknTextureRenderMeta.h>
namespace idk::vkn
{
	vk::AttachmentLoadOp MapLoadOp(LoadOp load_op);
	vk::AttachmentStoreOp MapStoreOp(StoreOp Store_op);

void SubPassConfig::SetDepthAttachment(index_t attachment_index, vk::ImageLayout use)
{
	depth = attachment_t{ attachment_index, Attachment{ use } };
}

void SubPassConfig::AddInputAttachment(index_t attachment_index, vk::ImageLayout use)
{
	inputs.emplace_back(attachment_t{ attachment_index, Attachment{ use } });
}

void SubPassConfig::AddOutputAttachment(index_t attachment_index, vk::ImageLayout use)
{
	inputs.emplace_back(attachment_t{ attachment_index, Attachment{ use } });
}

void SubPassConfig::BuildSubpass()
{
	uint32_t num_col = static_cast<uint32_t>(output.size());

	uint32_t num_in = static_cast<uint32_t>(inputs.size());


	auto& attachments_ref = data.attachments_ref = vector< vk::AttachmentReference>(num_col);
	auto& input_ref = data.input_ref = vector< vk::AttachmentReference>(num_in);


	auto& depth_attachment_ref = data.depth_attachment_ref = vk::AttachmentReference{};
	//TODO: Figure out how to deal with DepthStencil since vulkan only accepts them together, they can't be separate.
	
	ProcessAttachments(num_col, output, attachments_ref);

	vk::AttachmentReference*& pdepth_ref = data.pdepth_ref = nullptr;
	//for (auto& depth_attachment : depth_attachments)
	if (depth)
	{
		auto& [index, att_config] = *depth;
		depth_attachment_ref =
			vk::AttachmentReference{
			num_col
			,att_config.use
		};
		pdepth_ref = &depth_attachment_ref;
	}
	ProcessAttachments(num_in, inputs, input_ref);

	data.subpass =
		vk::SubpassDescription{
		vk::SubpassDescriptionFlags{}
		,vk::PipelineBindPoint::eGraphics
		,hlp::arr_count(input_ref),(hlp::arr_count(input_ref)) ? std::data(input_ref) : nullptr
		,hlp::arr_count(attachments_ref),(hlp::arr_count(attachments_ref)) ? std::data(attachments_ref) : nullptr
		,nullptr
		,pdepth_ref
	};

}

vk::SubpassDescription SubPassConfig::GetSubpassDesc() const
{
	return data.subpass;
}

std::pair<vk::AccessFlags, vk::PipelineStageFlags> SubPassConfig::GetDep()
{
	std::pair<vk::AccessFlags, vk::PipelineStageFlags> pair{};
	auto& [dest_access_flag, dest_mask] = pair;

	if (data.attachments_ref.size())
	{
		dest_mask |= vk::PipelineStageFlagBits::eColorAttachmentOutput;
		dest_access_flag = vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite;
	}


	if (data.pdepth_ref)
	{
		dest_mask |= vk::PipelineStageFlagBits::eEarlyFragmentTests | vk::PipelineStageFlagBits::eLateFragmentTests;
		dest_access_flag |= vk::AccessFlagBits::eDepthStencilAttachmentRead | vk::AccessFlagBits::eDepthStencilAttachmentWrite;
	}
	return pair;
}

void SubPassConfig::ProcessAttachments(uint32_t num, vector<attachment_t>& output, vector<vk::AttachmentReference>& attachment_refs)
{

	uint32_t i = 0;
	for (i = 0; i < num; ++i)
	{
		auto& [index, att_config] = output[i];
		attachment_refs[i] =
			vk::AttachmentReference{
			i
			,att_config.use
		};
	}
}

index_t RenderPassInfo::RegisterAttachment(AttachmentInfo attachment, vk::ImageLayout in, vk::ImageLayout out)
{
	auto index = hlp::arr_count(attachments);
	attachments.emplace_back(attachment);
	AddAttachment(attachment, attachment_desc, in, out);
	return index;
}

uint32_t RenderPassInfo::RegisterSubpass(SubPassConfig config)
{
	config.BuildSubpass();
	auto size = static_cast<uint32_t>(subpasses.size());
	subpasses.emplace_back(std::move(config));
	return size;
}

void RenderPassInfo::AddDependency(uint32_t src, uint32_t dest)
{
	auto [src_access_flag, src_mask] = subpasses[src].GetDep();
	auto [dest_access_flag, dest_mask] = subpasses[dest].GetDep();
	dependencies.emplace_back(
		vk::SubpassDependency
		{
			src                   //src
			,dest				   //dest
		, src_mask
		, dest_mask
		, src_access_flag
		, dest_access_flag
		});
}

vk::RenderPassCreateInfo RenderPassInfo::BuildRenderPass()
{
	for (auto& subpass : subpasses)
	{
		sub.emplace_back(subpass.data.subpass);
	}

	return vk::RenderPassCreateInfo
	{
		vk::RenderPassCreateFlags{}
		,hlp::arr_count(attachment_desc),(hlp::arr_count(attachment_desc)) ? std::data(attachment_desc) : nullptr
		,hlp::arr_count(sub),std::data(sub)
		,hlp::arr_count(dependencies),std::data(dependencies)
	};
}

void RenderPassInfo::AddAttachment(AttachmentInfo attachment, vector<vk::AttachmentDescription>& attachment_desc, vk::ImageLayout in, vk::ImageLayout out)
{
	auto& col_attachment = attachment;
	auto& vk_att = col_attachment;

	attachment_desc.emplace_back(vk::AttachmentDescription{
		vk::AttachmentDescriptionFlags{}
		, MapFormat(vk_att.internal_format)
		, vk::SampleCountFlagBits::e1
		, MapLoadOp(vk_att.load_op)
		, MapStoreOp(vk_att.store_op)
		, vk::AttachmentLoadOp::eDontCare//MapLoadOp (vk_att.stencil_load_op )
		, vk::AttachmentStoreOp::eDontCare//MapStoreOp(vk_att.stencil_store_op)
		, in
		, out
		});
}

}