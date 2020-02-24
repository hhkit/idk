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
	void ResourceLifetimeManager::DebugArrange(FrameGraphResourceManager& rsc_manager) const
	{
		using Debug =vector<std::tuple<string, fgr_id, index_span>>;
		//struct Debug
		//{
		//	 resources;
		//};
		lazy_vector<Debug> test;
		for (auto& [r_id, index] : map)
		{
			auto& rsc_lifetime = resource_lifetimes.at(index);
			auto itr = resource_alias.find(r_id);
			
			auto derp = rsc_manager.GetResourceDescription(r_id);
			auto actual_index = itr->second;
			test[actual_index].emplace_back(derp->name, r_id,index_span{ rsc_lifetime.start,rsc_lifetime.end });
		}
		test.clear();
	}

	bool ResourceLifetimeManager::overlap_lifetime(const actual_resource_t& rsc, order_t start, order_t end)
	{
		return !((rsc.end<start) | (rsc.start>end));
	}

	void ResourceLifetimeManager::Alias(fgr_id id, order_t start, order_t end, actual_resource_t& rsc)
	{
		resource_alias[id] = rsc.id;
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