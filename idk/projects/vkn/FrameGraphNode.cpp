#include "pch.h"
#include "FrameGraphNode.h"
#include "FrameGraphCopyResource.h"
namespace idk::vkn
{
	span<const FrameGraphCopyResource> FrameGraphNode::GetCopySpan() const { return (copy_buffer) ? copied_resources.to_span(*copy_buffer) : span<const FrameGraphCopyResource>{nullptr,nullptr}; }
	bool FrameGraphNode::resource_present(index_span span, FrameGraphResource rsc) const
	{
		auto begin = buffer->begin() + span._begin;
		auto end = buffer->begin() + span._end;
		auto itr = std::find(begin, end, rsc);
		return itr != end;
	}
	bool FrameGraphNode::Reads(FrameGraphResource rsc) const
	{
		return resource_present(input_resources, rsc);
	}

	bool FrameGraphNode::Writes(FrameGraphResource rsc) const
	{
		return resource_present(output_resources, rsc);
	}

	bool FrameGraphNode::Modifies(FrameGraphResource rsc) const
	{
		return resource_present(modified_resources, rsc);
	}

}