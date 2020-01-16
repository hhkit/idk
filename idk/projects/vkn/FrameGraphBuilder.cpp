#include "pch.h"
#include "FrameGraphBuilder.h"
namespace idk::vkn
{

	FrameGraphResource FrameGraphBuilder::CreateTexture(TextureDescription desc)
	{
		return rsc_manager.CreateTexture(desc);
	}

	FrameGraphResourceReadOnly FrameGraphBuilder::read(FrameGraphResource in_rsc)
	{
		auto rsc = rsc_manager.Rename(in_rsc);
		curr_rsc.input_resources.emplace_back(in_rsc);
		return rsc;
	}

	FrameGraphResourceMutable FrameGraphBuilder::write(FrameGraphResource target_rsc, WriteOptions opt)
	{
		auto rsc = rsc_manager.Rename(target_rsc);
		if (!opt.clear)
			curr_rsc.input_resources.emplace_back(target_rsc);
		else
			curr_rsc.modified_resources.emplace_back(target_rsc);
		curr_rsc.output_resources.emplace_back(rsc);
		return rsc;
	}

	void FrameGraphBuilder::set_input_attachment(FrameGraphResourceReadOnly in_rsc, uint32_t attachment_index, AttachmentDescription attachment_desc)
	{
		auto size = std::max(curr_rsc.input_attachments.size(), static_cast<size_t>(attachment_index + 1));
		curr_rsc.input_attachments.resize(size);
		curr_rsc.input_attachments[attachment_index] = { in_rsc.id,attachment_desc };
	}

	void FrameGraphBuilder::set_output_attachment(FrameGraphResourceMutable out_rsc, uint32_t attachment_index, AttachmentDescription attachment_desc)
	{
		auto size = std::max(curr_rsc.output_attachments.size(), static_cast<size_t>(attachment_index + 1));
		curr_rsc.output_attachments.resize(size);
		curr_rsc.output_attachments[attachment_index] = { out_rsc.id,attachment_desc };
	}

	void FrameGraphBuilder::set_depth_stencil_attachment(FrameGraphResourceMutable out_rsc, uint32_t attachment_index, AttachmentDescription attachment_desc)
	{
		
	}

	void FrameGraphBuilder::BeginNode()
	{
		curr_rsc.reset();
	}

	FrameGraphNode FrameGraphBuilder::EndNode()
	{
		auto id = NextID();
		for (auto& rsc : curr_rsc.output_resources)
		{
			origin_nodes.emplace(rsc.id, id);
		}
		auto input_span = consumed_resources.StoreResources(curr_rsc.input_resources);
		auto read_span = consumed_resources.StoreResources(curr_rsc.read_resources);
		auto output_span = consumed_resources.StoreResources(curr_rsc.output_resources);
		auto modified_span = consumed_resources.StoreResources(curr_rsc.modified_resources);

		return FrameGraphNode{ id,&consumed_resources.resources,input_span,read_span,output_span,modified_span,curr_rsc.input_attachments,curr_rsc.output_attachments,curr_rsc.depth_attachment };
	}

	void FrameGraphBuilder::CurrResources::reset()
	{
		input_resources.clear();
		output_resources.clear();
		modified_resources.clear();

		input_attachments.clear();
		output_attachments.clear();
	}

	index_span NodeBuffer::StoreResources(vector<FrameGraphResource>& rsc)
	{
		index_span result{ resources.size(),resources.size() };
		resources.reserve(rsc.size() + resources.size());
		std::copy(rsc.begin(), rsc.end(), resources.end());
		result.end = resources.size();
		return result;
	}

}