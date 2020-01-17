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
		renamed_resources.emplace(next_id, rsc.id);
		return FrameGraphResource{ next_id };
	}

	FrameGraphResource FrameGraphResourceManager::WriteRename(FrameGraphResource rsc)
	{
		auto renamed = Rename(rsc);
		write_renamed.emplace(rsc.id,renamed.id);
		return renamed;
	}

	FrameGraphResource FrameGraphResourceManager::WriteRenamed(FrameGraphResource rsc)const
	{
		return FrameGraphResource{ write_renamed.find(rsc.id)->second };
	}


	bool FrameGraphResourceManager::IsWriteRenamed(FrameGraphResource rsc) const
	{
		return write_renamed.find(rsc.id) != write_renamed.end();
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

	FrameGraphResourceManager::actual_resource_t& FrameGraphResourceManager::GetVar(fgr_id rsc)
	{
		return concrete_resources[resource_map.find(rsc)->second];
	}

	std::optional<fgr_id> FrameGraphResourceManager::GetPrevious(fgr_id curr) const
	{
		auto itr = renamed_resources.find(curr);
		std::optional<fgr_id> result{};
		if (itr != renamed_resources.end())
			result = itr->first;
		return result;
	}
}

