#pragma once
#include "idk.h"
#include "FrameGraphNode.h"
#include "FrameGraphResource.h"
#include "AttachmentDescription.h"
#include "FrameGraphResourceManager.h"
namespace idk::vkn
{
	struct NodeBuffer
	{
		vector<FrameGraphResource> resources;
		index_span StoreResources(vector<FrameGraphResource>& rsc);
	};
	struct WriteOptions
	{
		bool clear = true;
	};

	struct FrameGraphBuilder
	{
		//Keep track of virtual resources
		//AttachmentInfo CreateTexture(AttachmentDescription desc);
		//AttachmentInfo CreateTexture(const Texture& texture);
		fg_id NextID();

		FrameGraphResource CreateTexture(AttachmentDescription desc);
		FrameGraphResourceReadOnly read(FrameGraphResource in_rsc);
		FrameGraphResourceMutable write(FrameGraphResource target_rsc, WriteOptions opt = {});

		void set_input_attachment(FrameGraphResourceReadOnly in_rsc, uint32_t attachment_index);
		void set_output_attachment(FrameGraphResourceMutable out_rsc, uint32_t attachment_index);

		void BeginNode();
		FrameGraphNode EndNode();

		FrameGraphResourceManager rsc_manager;


		hash_table<fgr_id, fg_id> origin_nodes;

		
		NodeBuffer consumed_resources;
	private:
		struct CurrResources
		{
			vector<FrameGraphResource> input_resources;
			vector<FrameGraphResource> output_resources;
			vector<FrameGraphResource> modified_resources;

			vector<std::optional<fgr_id>> input_attachments;
			vector<std::optional<fgr_id>> output_attachments;
			void reset();
		};

		//Consumed resources
		CurrResources curr_rsc;
	};
}