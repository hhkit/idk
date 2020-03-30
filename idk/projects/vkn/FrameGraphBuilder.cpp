#include "pch.h"
#include "FrameGraphBuilder.h"
namespace idk::vkn
{

	namespace validate
	{
		bool OutputAttachment(fgr_id att_id,std::optional<fgr_id> before_write, AttachmentDescription att, span<const FrameGraphResource> input_resources)
		{
			bool result = true;
			if (att.load_op == vk::AttachmentLoadOp::eLoad && before_write)
			{
				att_id = *before_write;
				result = false;
				for (auto rsc : input_resources)
				{
					if (rsc.id == att_id)
					{
						result = true;
						break;
					}
				}
			}
			return result;
		}
	}

	fg_id FrameGraphBuilder::NextID()
	{
		return _fgid_generator.gen_next();
	}
	void FrameGraphBuilder::ResetIDs()
	{
		_fgid_generator.reset_ids();
	}
	void FrameGraphBuilder::Reset()
	{
		ResetIDs();
		consumed_resources.Reset();
		origin_nodes.clear();
		input_origin_nodes.clear();
		_region_name.clear();
	}
	FrameGraphResource FrameGraphBuilder::CreateTexture(TextureDescription desc)
	{
		desc.name = curr_rsc.name +":"+ desc.name;
		auto rsc = rsc_manager.CreateTexture(desc);
		curr_rsc.output_resources.emplace_back(rsc);
		return rsc;
	}

	FrameGraphResourceReadOnly FrameGraphBuilder::read(FrameGraphResource in_rsc,[[maybe_unused]] bool may_shader_sample)
	{
		auto rsc = rsc_manager.Rename(in_rsc);
		curr_rsc.input_resources.emplace_back(in_rsc);
		return rsc;
	}

	FrameGraphResourceMutable FrameGraphBuilder::write(FrameGraphResource target_rsc, WriteOptions opt)
	{
		//Ensure that a resource is never written to twice. Manually create order by writing to the result of another write only once.
		while (rsc_manager.IsWriteRenamed(target_rsc))
		{
			LOG_WARNING_TO(LogPool::GFX, "Warning: Writing to resource %ull twice.",target_rsc.id);
			target_rsc = rsc_manager.WriteRenamed(target_rsc);
		}
		auto rsc = rsc_manager.WriteRename(target_rsc);
		if (!opt.clear)
			curr_rsc.input_resources.emplace_back(target_rsc);
		
		curr_rsc.modified_resources.emplace_back(target_rsc); 
		curr_rsc.output_resources.emplace_back(rsc);
		return rsc;
	}

	FrameGraphResource FrameGraphBuilder::copy(FrameGraphResource target_rsc, CopyOptions opt)
	{
		auto src_id = target_rsc.id;
		auto copy_desc = rsc_manager.GetResourceDescription(src_id);
		FrameGraphResource result = target_rsc;
		if (copy_desc)
		{
			auto usage = copy_desc->usage;
			copy_desc->usage = usage | vk::ImageUsageFlagBits::eTransferSrc;
			rsc_manager.UpdateResourceDescription(src_id, *copy_desc);
			if (copy_desc->actual_rsc)
			{
				copy_desc->size = copy_desc->actual_rsc->as<VknTexture>().Size();
				copy_desc->actual_rsc = {};
			}
			copy_desc->usage = usage | vk::ImageUsageFlagBits::eTransferDst;
			result= CreateTexture(*copy_desc);
			//result = write(result, WriteOptions{ .clear = false }); //Create already puts it into output resources
			curr_rsc.copies.emplace_back(FrameGraphCopyResource{target_rsc,result,opt});
			read(target_rsc,false);
		}
		return result;
	}

	void FrameGraphBuilder::NoRenderPass(bool no_render_pass)
	{
		curr_rsc.no_rp = no_render_pass;
	}

	void FrameGraphBuilder::set_input_attachment(FrameGraphResourceReadOnly in_rsc, uint32_t attachment_index, AttachmentDescription attachment_desc)
	{
		auto size = std::max(curr_rsc.input_attachments.size(), static_cast<size_t>(attachment_index + 1));
		curr_rsc.input_attachments.resize(size);
		curr_rsc.input_attachments[attachment_index] = { in_rsc.id,attachment_desc };
		rsc_manager.MarkUsage(in_rsc.id, vk::ImageUsageFlagBits::eInputAttachment);
	}
//#pragma optimize("",off)

	void FrameGraphBuilder::set_output_attachment(FrameGraphResourceMutable out_rsc, uint32_t attachment_index, AttachmentDescription attachment_desc)
	{
		auto read = rsc_manager.BeforeWriteRenamed(out_rsc);
		if (!validate::OutputAttachment(out_rsc.id,read, attachment_desc, curr_rsc.input_resources))
		{
			LOG_ERROR_TO(LogPool::GFX, "Attachment is loading from a resource without calling read on it. \nYou probably forgot to specify clear = false when calling write.");
		}
		auto size = std::max(curr_rsc.output_attachments.size(), static_cast<size_t>(attachment_index + 1));
		curr_rsc.output_attachments.resize(size);
		curr_rsc.output_attachments[attachment_index] = { out_rsc.id,attachment_desc };
	}

	void FrameGraphBuilder::set_depth_stencil_attachment(FrameGraphResourceMutable out_rsc, AttachmentDescription attachment_desc)
	{
		curr_rsc.depth_attachment = {out_rsc.id,attachment_desc};
		rsc_manager.MarkUsage(out_rsc.id, vk::ImageUsageFlagBits::eDepthStencilAttachment);
	}

	void FrameGraphBuilder::MarkNodeRegion(string name)
	{
		_region_name = std::move(name);
	}

	void FrameGraphBuilder::BeginNode(string name)
	{
		curr_rsc.reset();
		curr_rsc.name = std::move(_region_name+":"+name);
	}

	FrameGraphNode FrameGraphBuilder::EndNode()
	{
		auto id = NextID();
		for (auto& rsc : curr_rsc.output_resources)
		{
			origin_nodes.emplace(rsc.id, id);
		}
		for (auto& rsc : curr_rsc.input_resources) //Will be transitioned by the resources
		{
			input_origin_nodes.emplace(rsc.id, id);
		}
		auto input_span    = consumed_resources.StoreResources(curr_rsc.input_resources);
		auto read_span     = consumed_resources.StoreResources(curr_rsc.read_resources);
		auto output_span   = consumed_resources.StoreResources(curr_rsc.output_resources);
		auto modified_span = consumed_resources.StoreResources(curr_rsc.modified_resources);
		auto copied_span   = consumed_resources.StoreCopies(curr_rsc.copies);

		return FrameGraphNode{ id,std::move(curr_rsc.name),&consumed_resources.resources,&consumed_resources.copies,input_span,read_span,output_span,modified_span,copied_span,curr_rsc.input_attachments,curr_rsc.output_attachments,curr_rsc.depth_attachment,curr_rsc.no_rp };
	}

	std::optional<fg_id> FrameGraphBuilder::GetSourceNode(fgr_id aliased_rsc) const
	{
		std::optional < fg_id> result{};
		auto rsc = rsc_manager.GetOriginal(aliased_rsc);
		auto itr = origin_nodes.find(rsc);
		if (itr != origin_nodes.end())
			result = itr->second;
		return result;
	}

	void FrameGraphBuilder::PreObject::reset()
	{
		no_rp = false;
		input_resources.clear();
		read_resources.clear();
		output_resources.clear();
		modified_resources.clear();
		copies.clear();

		input_attachments.clear();
		output_attachments.clear();
		depth_attachment.reset();
	}

	template<typename Buffer,typename Src>
	index_span StoreResourceInBuffer(Buffer& buffer, Src& rsc)
	{
		const  auto start_sz = buffer.size();
		index_span result{ start_sz,start_sz };
		buffer.reserve(rsc.size() + start_sz);
		std::copy(rsc.begin(), rsc.end(), std::back_inserter(buffer));
		result._end = buffer.size();
		return result;
	}

	index_span NodeBuffer::StoreResources(vector<FrameGraphResource>& rsc)
	{
		return StoreResourceInBuffer(resources, rsc);
	}


	index_span NodeBuffer::StoreCopies(vector<FrameGraphCopyResource>& rsc)
	{
		auto& buffer = copies;
		return StoreResourceInBuffer(buffer, rsc);
	}

	void NodeBuffer::Reset()
	{
		resources.clear();
		copies.clear();
	}

}