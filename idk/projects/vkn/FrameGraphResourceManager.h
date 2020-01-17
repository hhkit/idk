#pragma once
#include <ds/dual_set.h>
#include "FrameGraphNode.h"
#include "FrameGraphResource.h"
#include <vkn/AttachmentDescription.h>
#include <vkn/VknTexture.h>
namespace idk::vkn
{
	//All the necessary information to transition a resource to its target configuration
	struct TransitionInfo
	{

	};

	struct FrameGraphResourceManager
	{
		using rsc_index_t = size_t;
		using actual_rsc_index_t = size_t;
		using actual_resource_t = variant<unique_ptr<VknTexture>>;


		//Instantiates an actual resource using base's configuration and associate it with unique_id
		void Instantiate(size_t unique_id, fgr_id base);

		//Associate fgr_id with unique_id
		void Alias(size_t unique_id, fgr_id id);

		TransitionInfo TransitionInfo(const FrameGraphResource& rsc);

		FrameGraphResource CreateTexture(TextureDescription dsc);
		FrameGraphResource Rename(FrameGraphResource rsc);
		FrameGraphResource WriteRename(FrameGraphResource rsc);
		FrameGraphResource WriteRenamed(FrameGraphResource rsc)const;
		bool IsWriteRenamed(FrameGraphResource rsc)const;
		string_view Name(FrameGraphResource fg)const;

		bool IsCompatible(fgr_id lhs, fgr_id rhs)const;

		template<typename ActualResource>
		unique_ptr<ActualResource>& Get(fgr_id rsc)
		{
			return std::get<unique_ptr<ActualResource>>(GetVar(rsc));
		}

		actual_resource_t& GetVar(fgr_id rsc);
		//Generate the next id.
		fgr_id NextID();

		std::optional<fgr_id> GetPrevious(fgr_id curr)const;

		vector<TextureDescription> resources;

		vector<actual_resource_t> concrete_resources;
		hash_table<fgr_id, actual_rsc_index_t> resource_map;

		hash_table<fgr_id, rsc_index_t> resource_handles;
		//old to new(first), new to old(second)
		dual_set<fgr_id, fgr_id> write_renamed;
		//new to old(second)
		hash_table<fgr_id, fgr_id> renamed_resources;
	};

}
MARK_NON_COPY_CTORABLE(idk::vkn::FrameGraphResourceManager::actual_resource_t);