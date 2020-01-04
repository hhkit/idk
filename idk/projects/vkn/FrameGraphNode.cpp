#include "pch.h"
#include "FrameGraphNode.h"
namespace idk::vkn
{
	bool FrameGraphNode::resource_present(index_span span, FrameGraphResource rsc) const
	{
		auto begin = buffer->begin() + span.begin;
		auto end = buffer->begin() + span.end;
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