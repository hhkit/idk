#include "pch.h"
#include "FrameGraphResourceManager.h"
#include <gfx/Texture.h>
#include <ds/dual_set.inl>

#include <vkn/VknTextureLoader.h>
#include <vkn/VulkanView.h>
#include <res/ResourceHandle.inl>
namespace idk::vkn
{
	bool ValidateUsage(const TextureDescription& desc,vk::ImageUsageFlags usage)
	{
		bool result = true;
		switch (desc.format)
		{
		case vk::Format::eD16Unorm:
		case vk::Format::eD16UnormS8Uint:
		case vk::Format::eD24UnormS8Uint:
		case vk::Format::eD32Sfloat:
		case vk::Format::eD32SfloatS8Uint:
			break;
		default:
			if ((desc.usage | usage) & vk::ImageUsageFlagBits::eDepthStencilAttachment)
			{
				LOG_ERROR_TO(LogPool::GFX, "Attempting to use a non-depth-stencil texture as a depth-stencil attachment");
				result=false;
			}
			break;
		}
		return result;
	}
	VulkanView& View();
	FrameGraphResourceManager::FrameGraphResourceManager() = default;
	FrameGraphResourceManager::FrameGraphResourceManager(FrameGraphResourceManager&&)=default;
	FrameGraphResourceManager& FrameGraphResourceManager::operator=(FrameGraphResourceManager&&)=default;
	FrameGraphResourceManager::~FrameGraphResourceManager()=default;

	void FrameGraphResourceManager::Instantiate(size_t unique_id, fgr_id base)
	{
		concrete_resources.resize(std::max(unique_id+1, concrete_resources.size()));
		//Create Resource at the back of concrete_resources
		auto rsc_desc = GetResourceDescription(base);
		if (!rsc_desc)
		{
			LOG_WARNING_TO(LogPool::GFX, "Attempting to instantiate a resource without a valid description! base: %ull",base);
			return;
		}
		concrete_resources[unique_id]=InstantiateConcrete(*rsc_desc,true);
		
		resource_map[base] = unique_id;
	}
	void FrameGraphResourceManager::Alias(fgr_id id, fgr_id unique_id)
	{
		resource_map[id] = unique_id;
	}
	TransitionInfo FrameGraphResourceManager::TransitionInfo(const FrameGraphResource& rsc)
	{
		//TODO
		return vkn::TransitionInfo{};
	}

	void FrameGraphResourceManager::MarkUsage(fgr_id rsc_id, vk::ImageUsageFlags usage)
	{
		auto o_prsc = GetResourceDescriptionPtr(rsc_id);
		if (!o_prsc)
		{
			LOG_WARNING_TO(LogPool::GFX, "Attempting to use resource that has no description! rsc_id: %ull", rsc_id);
		}
		else
		{
			auto& desc = **o_prsc;
			if(ValidateUsage(desc,usage))
				(*o_prsc)->usage |= usage;
		}
	}
#pragma optimize ("",off)
	FrameGraphResource FrameGraphResourceManager::CreateTexture(TextureDescription dsc)
	{
		auto rsc_index = resources.size();
		resources.emplace_back(dsc);
		auto id = NextID();
		resource_handles.emplace(id, rsc_index);
		renamed_original.emplace(id, id);
		return FrameGraphResource{ id };
	}

	FrameGraphResource FrameGraphResourceManager::Rename(FrameGraphResource rsc)
	{
		auto next_id = NextID();
		auto itr = resource_handles.find(rsc.id);
		auto oitr = renamed_original.find(rsc.id);
		if (itr == resource_handles.end() || oitr ==resource_handles.end())
			throw;
		resource_handles.emplace(next_id, itr->second);
		renamed_resources.emplace(next_id, rsc.id);
		renamed_original.emplace(next_id, oitr->second);
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

	std::optional<fgr_id> FrameGraphResourceManager::BeforeWriteRenamed(FrameGraphResource rsc) const
	{
		std::optional<fgr_id> result = {};
		auto itr = write_renamed.find_second(rsc.id);
		if (itr != write_renamed.end())
			result = itr->first;
		return result;
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

		if (l_desc.actual_rsc || r_desc.actual_rsc)
			return false;

		[[maybe_unused]] string_view lhs_name = l_desc.name, rhs_name= r_desc.name;

		return l_desc.usage==r_desc.usage&&l_desc.format == r_desc.format && l_desc.type == r_desc.type && l_desc.layer_count == r_desc.layer_count && l_desc.aspect == r_desc.aspect;
	}

	FrameGraphResourceManager::actual_resource_t& FrameGraphResourceManager::GetVar(fgr_id rsc)
	{
		auto itr = resource_map.find(rsc);
		while (itr == resource_map.end())
		{
			auto renamed_itr = renamed_resources.find(rsc);
			if (renamed_itr != renamed_resources.end())
				itr = resource_map.find(renamed_itr->second);
		}
		return concrete_resources[itr->second];
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
		std::optional<fgr_id> result = curr;
		//auto wr = BeforeWriteRenamed(FrameGraphResource{ curr }); //If this was written to
		//if (wr)
		//{
		//	curr = *wr;
		//}
		{
			result = {};
			auto itr = renamed_resources.find(curr);
			if (itr != renamed_resources.end() && itr->second != curr)
				result = itr->second;
		}
		return result;
	}

	fgr_id FrameGraphResourceManager::GetOriginal(fgr_id curr) const
	{
		return renamed_original.find(curr)->second;
	}

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
	bool FrameGraphResourceManager::UpdateResourceDescription(fgr_id rsc_id, TextureDescription desc)
	{
		bool result=false;
		auto itr = resource_handles.find(rsc_id);
		if (itr != resource_handles.end())
		{
			if (ValidateUsage(desc, {}))
			{
				resources[itr->second] = desc;
				result = true;;
			}
		}
		return result;
	}
	FrameGraphResourceManager::actual_resource_t FrameGraphResourceManager::InstantiateConcrete(TextureDescription desc,bool is_shader_sampled)
	{
		FrameGraphResourceManager::actual_resource_t result{};

		if (!desc.actual_rsc)
		{
			desc.usage = mark_sampled(desc.usage, is_shader_sampled);
			result = pool.allocate_async(desc);
		}
		else
		{
			result = **desc.actual_rsc;
		}
		return result;
	}
	void FrameGraphResourceManager::FinishInstantiation()
	{
		this->pool.collate_async();
		for(auto& concrete_resource : concrete_resources)
		{
			if (concrete_resource.index() == index_in_variant_v < std::future<VknTextureView>, std::remove_reference_t<decltype(concrete_resource)>>)
			{
				auto future = std::move(std::get< std::future<VknTextureView>>(concrete_resource));
				auto result = future.get();
				concrete_resource = result;
			}
		}
	}
	void FrameGraphResourceManager::Reset()
	{
		ResetIDs();
		pool.reset_allocations();
		resources.clear();
		concrete_resources.clear();
		resource_map.clear();
		resource_handles.clear();
		renamed_original.clear();
		write_renamed.clear();
		renamed_resources.clear();
	}
}

