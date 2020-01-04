#pragma once
#include "FrameGraphNode.h"
#include "FrameGraphResource.h"
#include <vkn/AttachmentDescription.h>
namespace idk::vkn
{
	struct FrameGraphResourceManager
	{
		using rsc_index_t = size_t;
		using actual_rsc_index_t = size_t;
		using actual_resource_t = variant<Texture>;

		//Instantiates an actual resource using base's configuration and associate it with unique_id
		void Instantiate(size_t unique_id, fgr_id base);

		//Associate fgr_id with unique_id
		void Alias(size_t unique_id, fgr_id id);

		FrameGraphResource CreateTexture(AttachmentDescription dsc);
		FrameGraphResource Rename(FrameGraphResource rsc);
		string_view Name(FrameGraphResource fg)const;

		template<typename ActualResource>
		ActualResource& Get(FrameGraphResource rsc)
		{
			return std::get<ActualResource>(GetVar(rsc));
		}

		actual_resource_t& GetVar(FrameGraphResource rsc);
		//Generate the next id.
		fgr_id NextID();


		vector<AttachmentDescription> resources;

		vector<actual_resource_t> concrete_resources;
		hash_table<fgr_id, actual_rsc_index_t> resource_map;

		hash_table<fgr_id, rsc_index_t> resource_handles;
		//Old to new
		hash_table<fgr_id, fgr_id> renamed_resources;
	};

}