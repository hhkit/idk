#pragma once
#include "idk.h"
#include "FrameGraphNode.h"
#include "FrameGraphResource.h"
#include "AttachmentDescription.h"
#include "FrameGraphResourceManager.h"
#include <vkn/IdGenerator.h>
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
		void ResetIDs();
		void Reset();

		FrameGraphResource CreateTexture(TextureDescription desc);
		FrameGraphResourceReadOnly read(FrameGraphResource in_rsc,bool may_shader_sample=true);
		FrameGraphResourceMutable write(FrameGraphResource target_rsc, WriteOptions opt = {});

		void set_input_attachment(FrameGraphResourceReadOnly in_rsc, uint32_t attachment_index , AttachmentDescription attachment_desc);
		void set_output_attachment(FrameGraphResourceMutable out_rsc, uint32_t attachment_index, AttachmentDescription attachment_desc);
		void set_depth_stencil_attachment(FrameGraphResourceMutable out_rsc, AttachmentDescription attachment_desc);

		void BeginNode(string name);
		FrameGraphNode EndNode();

		FrameGraphResourceManager rsc_manager;


		hash_table<fgr_id, fg_id> origin_nodes;

		
		NodeBuffer consumed_resources;
	private:
		struct PreObject
		{
			string name;
			vector<FrameGraphResource> input_resources;
			vector<FrameGraphResource> read_resources;
			vector<FrameGraphResource> output_resources;
			vector<FrameGraphResource> modified_resources;

			vector<std::optional<FrameGraphAttachmentInfo>> input_attachments;
			vector<std::optional<FrameGraphAttachmentInfo>> output_attachments;
			std::optional<FrameGraphAttachmentInfo> depth_attachment;
			void reset();
		};

		//Consumed resources
		PreObject curr_rsc;
		hlp::IdGenerator<fg_id> _fgid_generator;
	};
}