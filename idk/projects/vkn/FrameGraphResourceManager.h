#pragma once
#include <ds/dual_set.h>
#include "FrameGraphNode.h"
#include "FrameGraphResource.h"
#include <vkn/AttachmentDescription.h>
#include <vkn/VknTexture.h>
#include <vkn/IdGenerator.h>
#include <vkn/VknTextureView.h>
#include <vkn/TexturePool.h>

#include <util/PoolContainer.h>

namespace idk::vkn
{
	//All the necessary information to transition a resource to its target configuration
	struct TransitionInfo
	{

	};



	class FrameGraphResourceManager
	{
	public:
		using rsc_index_t = size_t;
		using actual_rsc_index_t = size_t;
		using actual_resource_t = variant<VknTextureView,std::future<VknTextureView>>;

		FrameGraphResourceManager();
		FrameGraphResourceManager(const FrameGraphResourceManager&) = delete;
		FrameGraphResourceManager(FrameGraphResourceManager&&);
		FrameGraphResourceManager& operator=(const FrameGraphResourceManager&) = delete;
		FrameGraphResourceManager& operator=(FrameGraphResourceManager&&);
		~FrameGraphResourceManager();


		//Instantiates an actual resource using base's configuration and associate it with unique_id
		void Instantiate(size_t unique_id, fgr_id base);

		//returns the ratio of the virtual resource against its concrete resource size
		//components should never exceed 1
		vec2 ConcreteSizeRatio(fgr_id rsc)const;

		//Associate fgr_id with unique_id
		void Alias(fgr_id unique_id, fgr_id id);

		TransitionInfo TransitionInfo(const FrameGraphResource& rsc);

		void MarkUsage(fgr_id rsc_id,vk::ImageUsageFlags usage);

		FrameGraphResource CreateTexture(TextureDescription dsc);
		FrameGraphResource Rename(FrameGraphResource rsc);
		FrameGraphResource WriteRename(FrameGraphResource rsc);
		FrameGraphResource WriteRenamed(FrameGraphResource rsc)const;
		std::optional<fgr_id> BeforeWriteRenamed(FrameGraphResource rsc)const;
		bool IsWriteRenamed(FrameGraphResource rsc)const;
		string_view Name(FrameGraphResource fg)const;

		bool IsCompatible(fgr_id original, fgr_id new_)const;

		template<typename ActualResource>
		ActualResource& Get(fgr_id rsc)
		{
			return std::get<ActualResource>(GetVar(rsc));
		}
		template<typename ActualResource>
		const ActualResource& Get(fgr_id rsc)const
		{
			return std::get<ActualResource>(GetVar(rsc));
		}

		actual_resource_t& GetVar(fgr_id rsc);
		const actual_resource_t& GetVar(fgr_id rsc)const;
		//Generate the next id.
		fgr_id NextID();
		void ResetIDs();

		std::optional<fgr_id> GetPrevious(fgr_id curr)const;
		fgr_id GetOriginal(fgr_id curr)const;

		std::optional<TextureDescription> GetResourceDescription(fgr_id rsc_id)const;
		bool UpdateResourceDescription(fgr_id rsc_id, TextureDescription desc);

		actual_resource_t InstantiateConcrete(TextureDescription desc, bool is_shader_sampled);
		void FinishInstantiation();
		void Reset();

		PooledContainer < vector<TextureDescription>> resources;

		PooledContainer < vector<actual_resource_t> > concrete_resources;
		PooledContainer < hash_table<fgr_id, actual_rsc_index_t>> resource_map;

		PooledContainer<hash_table<fgr_id, rsc_index_t>> resource_handles;
		//old to new(first), new to old(second)
		dual_set<fgr_id, fgr_id> write_renamed;
		//new to old(second)
		PooledContainer<hash_table<fgr_id, fgr_id>> renamed_resources;
		PooledContainer<hash_table<fgr_id, fgr_id>> renamed_rsc_next;
		PooledContainer<hash_table<fgr_id, fgr_id>> renamed_original;
		struct OverrideTracker
		{
			hash_table<Guid, fgr_id> override_to_original;
			void Register(Guid guid, fgr_id id);
			std::optional<fgr_id> GetID(FrameGraphResourceManager& rsc_manager, Guid guid);
			void Reset();
		};
		OverrideTracker override_tracker;
		hlp::IdGenerator<fgr_id> _fgr_generator;
		TexturePool pool;
	private:
		fgr_id GetLatest(fgr_id id);
		std::optional<TextureDescription*> GetResourceDescriptionPtr(fgr_id rsc_id);
	};

}
MARK_NON_COPY_CTORABLE(idk::vkn::FrameGraphResourceManager::actual_resource_t);