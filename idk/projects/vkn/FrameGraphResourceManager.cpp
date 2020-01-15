#include "pch.h"
#include "FrameGraphResourceManager.h"
#include <gfx/Texture.h>
namespace idk::vkn
{

	FrameGraphResource FrameGraphResourceManager::CreateTexture(TextureDescription dsc)
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

	bool FrameGraphResourceManager::IsCompatible(fgr_id lhs, fgr_id rhs) const
	{
		auto l_itr = resource_handles.find(lhs);
		auto r_itr = resource_handles.find(rhs);
		if (l_itr == r_itr)
			return true;
		auto& l_desc = resources[l_itr->second], &r_desc = resources[r_itr->second];

		return l_desc.format == r_desc.format && l_desc.type == r_desc.type && l_desc.layer_count == r_desc.layer_count && l_desc.aspect == r_desc.aspect;
	}

	FrameGraphResourceManager::actual_resource_t& FrameGraphResourceManager::GetVar(FrameGraphResource rsc)
	{
		return concrete_resources[resource_map.find(rsc.id)->second];
	}

}