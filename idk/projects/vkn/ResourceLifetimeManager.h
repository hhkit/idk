#pragma once
#include "FrameGraphNode.h"
#include "FrameGraphResource.h"
namespace idk::vkn
{
	struct ResourceLifetime
	{
		fg_id start, end; //Fixed order
		vector<fg_id> inbetween; //Undetermined order
	};
	struct ResourceLifetimeManager
	{
		using lifetime_index = size_t;
		vector< ResourceLifetime> resource_lifetimes;
		hash_table<fgr_id, lifetime_index> map;

		void ClearLifetimes();

		void EndLifetime(fgr_id rsc_id, const fg_id& node_id);
		void InbetweenLifetime(fgr_id rsc_id, const fg_id& node_id);
		void StartLifetime(fgr_id rsc_id, const fg_id& node_id);

		using order_t = size_t;
		using actual_resource_id = size_t;
		struct actual_resource_t
		{
			actual_resource_id id;
			fgr_id base_rsc;
			order_t start, end;
		};

		void ExtendLifetime(fgr_id rsc_id, order_t order);
		template<typename Func>
		void CombineAllLifetimes(Func&& compatibility_checker)
		{
			for (auto& [id, index] : map)
			{
				auto& lifetime = resource_lifetimes[index];
				CombineLifetimes(id, lifetime.start, lifetime.end, compatibility_checker);
			}
		}

		span<const actual_resource_t> GetActualResources()const;
		const hash_table<fgr_id, actual_resource_id> Aliases()const;

	private:
		auto NewNode(fgr_id r_id)
		{
			auto index = resource_lifetimes.size();
			resource_lifetimes.emplace_back(ResourceLifetime{ {std::numeric_limits<decltype(ResourceLifetime::start)>::max()},{std::numeric_limits<decltype(ResourceLifetime::start)>::min()},{} });
			return map.emplace(r_id, index).first;
		}
		auto& GetOrCreate(fgr_id r_id)
		{
			auto itr = map.find(r_id);
			if (itr == map.end())
			{
				itr = NewNode(r_id);
			}
			return resource_lifetimes.at(itr->second);
		}

		template<typename Func>
		void CombineLifetimes(fgr_id id, order_t start, order_t end, Func&& is_compatible)
		{
			for (auto& concrete_resource : concrete_resources)
			{
				if (!overlap_lifetime(concrete_resource, start, end) && is_compatible(id, concrete_resource.base_rsc))
				{
					Alias(id, start, end, concrete_resource);
					return;
				}
			}
			CreateResource(id, start, end);
		}

		bool overlap_lifetime(const actual_resource_t& rsc, order_t start, order_t end);
		void Alias(fgr_id id, order_t start, order_t end, actual_resource_t& rsc);
		actual_resource_id NewActualRscId();
		void CreateResource(fgr_id id, order_t start, order_t end);

		vector<actual_resource_t> concrete_resources;
		hash_table<fgr_id, actual_resource_id> resource_alias;
	};


}