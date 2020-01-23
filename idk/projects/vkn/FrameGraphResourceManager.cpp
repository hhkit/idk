#include "pch.h"
#include "FrameGraphResourceManager.h"
#include <gfx/Texture.h>
#include <ds/dual_set.inl>

#include <vkn/VknTextureLoader.h>
#include <vkn/VulkanView.h>
#include <res/ResourceHandle.inl>
namespace idk::vkn
{
	VulkanView& View();
	FrameGraphResourceManager::FrameGraphResourceManager() = default;
	FrameGraphResourceManager::FrameGraphResourceManager(FrameGraphResourceManager&&)=default;
	FrameGraphResourceManager& FrameGraphResourceManager::operator=(FrameGraphResourceManager&&)=default;
	FrameGraphResourceManager::~FrameGraphResourceManager()=default;

	void FrameGraphResourceManager::Instantiate(fgr_id unique_id, fgr_id base)
	{

		auto index = concrete_resources.size();
		//Create Resource at the back of concrete_resources
		auto rsc_desc = GetResourceDescription(base);
		if (!rsc_desc)
		{
			LOG_WARNING_TO(LogPool::GFX, "Attempting to instantiate a resource without a valid description! base: %ull",base);
			return;
		}
		concrete_resources.emplace_back(InstantiateConcrete(*rsc_desc,true));
		
		resource_map[unique_id] = index;
	}
	void FrameGraphResourceManager::Alias(fgr_id unique_id, fgr_id id)
	{
		resource_map[unique_id] = resource_map.find(id)->second;
	}
	TransitionInfo FrameGraphResourceManager::TransitionInfo(const FrameGraphResource& rsc)
	{
		//TODO
		return vkn::TransitionInfo{};
	}
#pragma optimize("",off)
	void FrameGraphResourceManager::MarkUsage(fgr_id rsc_id, vk::ImageUsageFlags usage)
	{
		auto o_prsc = GetResourceDescriptionPtr(rsc_id);
		if (!o_prsc)
		{
			LOG_WARNING_TO(LogPool::GFX, "Attempting to use resource that has no description! rsc_id: %ull", rsc_id);
		}
		else
		{
			(*o_prsc)->usage |= usage;
		}
	}
	FrameGraphResource FrameGraphResourceManager::CreateTexture(TextureDescription dsc)
	{
		auto rsc_index = resources.size();
		resources.emplace_back(dsc);
		auto id = NextID();
		resource_handles.emplace(id, rsc_index);
		return FrameGraphResource{ id };
	}

	FrameGraphResource FrameGraphResourceManager::Rename(FrameGraphResource rsc)
	{
		auto next_id = NextID();
		auto itr = resource_handles.find(rsc.id);
		if (itr == resource_handles.end())
			throw;
		resource_handles.emplace(next_id, itr->second);
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

	fgr_id FrameGraphResourceManager::NextID()
	{
		//TODO
		return _fgr_generator.gen_next();
	}

	void FrameGraphResourceManager::ResetIDs()
	{
		_fgr_generator.reset_ids();
		pool.reset_allocations();
	}

	std::optional<fgr_id> FrameGraphResourceManager::GetPrevious(fgr_id curr) const
	{
		auto itr = renamed_resources.find(curr);
		std::optional<fgr_id> result{};
		if (itr != renamed_resources.end())
			result = itr->first;
		return result;
	}
#pragma optimize("",off)
	std::optional<TextureDescription*> FrameGraphResourceManager::GetResourceDescriptionPtr(fgr_id rsc_id)
	{
		std::optional<TextureDescription*> result{};
		auto itr = resource_handles.find(rsc_id);
		if (itr != resource_handles.end())
		{
			result = &resources[itr->second];
		}
		return result;
	}
	std::optional<TextureDescription> FrameGraphResourceManager::GetResourceDescription(fgr_id rsc_id) const
	{
		std::optional<TextureDescription> result{};
		auto itr = resource_handles.find(rsc_id);
		if (itr != resource_handles.end())
		{
			result = resources[itr->second];
		}
		return result;
	}
	FrameGraphResourceManager::actual_resource_t FrameGraphResourceManager::InstantiateConcrete(TextureDescription desc,bool is_shader_sampled)
	{
		FrameGraphResourceManager::actual_resource_t result{};

		if (!desc.actual_rsc)
		{
			desc.usage = mark_sampled(desc.usage, is_shader_sampled);
			result = pool.allocate(desc);
		}
		else
		{
			result = **desc.actual_rsc;
		}
		return result;
	}
}

