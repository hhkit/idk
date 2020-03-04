#pragma once
#include <idk.h>
#include "FrameGraphResource.h"
#include "AttachmentDescription.h"
#include <ds/index_span.inl>
namespace idk::vkn
{
	using FrameGraphAttachmentInfo = std::pair<fgr_id, AttachmentDescription>;
	using fg_id = size_t;
	struct FrameGraphNode
	{
		fg_id id;

		string name;

		const vector<FrameGraphResource>* buffer;
		const vector<struct FrameGraphCopyResource>* copy_buffer;

		index_span input_resources; //The stuff that flow into the node here
		index_span read_resources; //The stuff that will be used here (paired)
		index_span output_resources;
		index_span modified_resources;
		
		index_span copied_resources;

		vector<std::optional<FrameGraphAttachmentInfo>> input_attachments;
		vector<std::optional<FrameGraphAttachmentInfo>> output_attachments;
		std::optional<FrameGraphAttachmentInfo> depth_stencil;

		bool skip_render_pass = false;
		
		auto GetReadSpan()const { return read_resources.to_span(*buffer); }
		auto GetInputSpan()const { return input_resources.to_span(*buffer); }
		auto GetOutputSpan()const { return output_resources.to_span(*buffer); }
		auto GetOutputAtt()const { return output_attachments; }
		span<const FrameGraphCopyResource> GetCopySpan()const;

		bool resource_present(index_span span, FrameGraphResource rsc)const;

		bool Reads(FrameGraphResource rsc)const;
		bool Writes(FrameGraphResource rsc)const;
		bool Modifies(FrameGraphResource rsc)const;

	};
}