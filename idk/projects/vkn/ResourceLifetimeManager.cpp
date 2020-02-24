#include "pch.h"
#include "ResourceLifetimeManager.h"
#include <ds/index_span.inl>
#include <ds/lazy_vector.h>
#include <vkn/FrameGraphResourceManager.h>
namespace idk::vkn
{
	void ResourceLifetimeManager::ClearLifetimes()
	{
		this->concrete_resources.clear();
		this->resource_alias.clear();
		this->map.clear();
		this->resource_lifetimes.clear();
	}
	void ResourceLifetimeManager::EndLifetime(fgr_id rsc_id, const fg_id& node_id)
	{
		GetOrCreate(rsc_id).end = node_id;
	}

	void ResourceLifetimeManager::InbetweenLifetime(fgr_id rsc_id, const fg_id& node_id)
	{
		GetOrCreate(rsc_id).inbetween.emplace_back(node_id);
	}

	void ResourceLifetimeManager::StartLifetime(fgr_id rsc_id, const fg_id& node_id)
	{
		GetOrCreate(rsc_id).start = node_id;
	}

	void ResourceLifetimeManager::ExtendLifetime(fgr_id rsc_id, order_t order)
	{
		auto& node = GetOrCreate(rsc_id);
		node.start = std::min(node.start, order);
		node.end = std::max(node.end, order);
	}

	span<const ResourceLifetimeManager::actual_resource_t> ResourceLifetimeManager::GetActualResources() const
	{
		return span<const actual_resource_t>{concrete_resources.data(), concrete_resources.data() + concrete_resources.size()};
	}

	const hash_table<fgr_id, ResourceLifetimeManager::actual_resource_id> ResourceLifetimeManager::Aliases() const
	{
		return resource_alias;
	}
#pragma optimize("",off)
	void ResourceLifetimeManager::DebugArrange(const FrameGraphResourceManager& rsc_manager) const
	{
		gfxdbg::FgRscLifetimes tmp;
		DebugArrange(tmp, rsc_manager);
		vector<std::pair<string, string>> derp;
		for (auto& [rsc, original] : mappings)
		{
			derp.emplace_back(rsc_manager.GetResourceDescription(rsc)->name, rsc_manager.GetResourceDescription(original)->name);
		}
		tmp.clear();
	}
	void ResourceLifetimeManager::DebugArrange(gfxdbg::FgRscLifetimes& dbg, const FrameGraphResourceManager& rsc_manager) const
	{
		using Debug = vector<std::tuple<string, fgr_id, index_span>>;
		auto& test = dbg;
		//struct Debug
		//{
		//	 resources;
		//};
		for (auto& line : dbg)
		{
			line.clear();
		}
		for (auto& [r_id, index] : map)
		{
			auto& rsc_lifetime = resource_lifetimes.at(index);
			auto itr = resource_alias.find(r_id);
			
			auto derp = rsc_manager.GetResourceDescription(r_id);
			auto actual_index = itr->second;
			test[actual_index].emplace_back(gfxdbg::DbgLifetime{ derp->name, r_id,rsc_lifetime.start,rsc_lifetime.end });
		}

		for (auto& [rsc_id, concrete_index] : resource_alias)
		{
			auto& resource = concrete_resources[concrete_index];
			auto base_desc = rsc_manager.GetResourceDescription(resource.base_rsc);
			auto rsc_desc = rsc_manager.GetResourceDescription(rsc_id);
			if (base_desc->format != rsc_desc->format)
			{
				throw;
			}
		}
	}

	bool ResourceLifetimeManager::overlap_lifetime(const actual_resource_t& rsc, order_t start, order_t end)
	{
		return !((rsc.end<start) | (rsc.start>end));
	}

	void ResourceLifetimeManager::Alias(fgr_id id, order_t start, order_t end, actual_resource_t& rsc)
	{
		resource_alias[id] = rsc.index;
		rsc.start = std::min(start, rsc.start);
		rsc.end = std::max(end, rsc.end);
	}

	ResourceLifetimeManager::actual_resource_id ResourceLifetimeManager::NewActualRscId()
	{
		return concrete_resources.size();
	}

	void ResourceLifetimeManager::CreateResource(fgr_id id, order_t start, order_t end)
	{
		auto& rsc = concrete_resources.emplace_back(actual_resource_t{ NewActualRscId(),id,start,end });
		Alias(id, start, end, rsc);
	}

}