#include "pch.h"
#include "FrameGraphResourceManager.h"
namespace idk::vkn
{

	FrameGraphResource FrameGraphResourceManager::CreateTexture(AttachmentDescription dsc)
	{
		auto rsc_index = resources.size();
		resources.emplace_back(dsc);
		resource_handles.emplace(NextID(), rsc_index);
		return FrameGraphResource{ rsc_index };
	}

	FrameGraphResource FrameGraphResourceManager::Rename(FrameGraphResource rsc)
	{
		auto next_id = NextID();
		resource_handles.emplace(next_id, resource_handles.find(rsc.id)->second);
		renamed_resources.emplace(rsc.id, next_id);
		return FrameGraphResource{ next_id };
	}

	string_view FrameGraphResourceManager::Name(FrameGraphResource fg) const
	{
		auto itr = resource_handles.find(fg.id);
		if (itr == resource_handles.end())
			return "";
		return resources.at(itr->second).name;
	}

	FrameGraphResourceManager::actual_resource_t& FrameGraphResourceManager::GetVar(FrameGraphResource rsc)
	{
		return concrete_resources[resource_map.find(rsc.id)->second];
	}

}