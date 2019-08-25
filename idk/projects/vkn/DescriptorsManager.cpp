#include "pch.h"
#include "DescriptorsManager.h"
#include <vkn/BufferHelpers.h>
#include <vkn/VulkanState.h>
#include <vkn/VulkanView.h>
//hash_table<vk::DescriptorSetLayout, DescriptorSetManager> ds_sets;

namespace idk::vkn
{
	DescriptorsManager::DescriptorsManager(VulkanView& view) :pools{view}
	{
	}
	DescriptorSetLookup  DescriptorsManager::Allocate(const hash_table<vk::DescriptorSetLayout, uint32_t>& allocations)
	{
		hash_table<vk::DescriptorSetLayout, DescriptorSets> result;
		////Get the amount of ds to be allocated
		//uint32_t req_allocs = 0;
		//for (auto& [layout, num_ds] : allocations)
		//{
		//	auto itr = ds_sets.find(layout);
		//	uint32_t num_req = num_ds;
		//	if (itr != ds_sets.end())
		//	{
		//		auto unused = itr->second.num_unused();
		//		num_req = std::max(num_ds, unused) - unused;
		//	}
		//	req_allocs += num_req;
		//}
		//auto pool = pools.Get(req_allocs);
		auto& m_device = pools.view.Device();
		auto& dispatcher = pools.view.Dispatcher();
		vector<bool> allocated(allocations.size(), false);
		uint32_t num_req = 0;
		do
		{
			uint32_t i = 0;
			num_req = 0;
			for (auto& [layout, num_ds] : allocations)
			{
				if (!allocated[i])
				{
					auto pool = pools.TryGet(num_ds);
					allocated[i++] = static_cast<bool>(pool);
					if (pool)
					{
						//TODO compute num_ds with layout's number of descriptors
						std::vector<vk::DescriptorSetLayout> layouts{ num_ds, layout };
						vk::DescriptorSetAllocateInfo allocInfo
						{
							*pool
							,hlp::arr_count(layouts)
							,std::data(layouts)
						};
						result[layout] = m_device->allocateDescriptorSets(allocInfo, dispatcher);
					}
					else
					{
						num_req += num_ds;
					}
				}
			}
			if (num_req)
				pools.Add(num_req);
		} while (num_req);
		return result;
	}

	void DescriptorsManager::Reset()
	{
		pools.ResetAll();
	}

	uint32_t DescriptorSets::size() const
	{
		return s_cast<uint32_t>(sets.size());
	}

	vk::DescriptorSet& DescriptorSets::GetNext()
	{
		return sets[curr_index++];
	}

	vector<vk::DescriptorSet>& DescriptorSets::operator=(vector<vk::DescriptorSet>&& rhs)
	{
		curr_index = 0;
		return sets = std::move(rhs);
	}
	vector<vk::DescriptorSet>& DescriptorSets::operator=(const vector<vk::DescriptorSet>& rhs)
	{
		curr_index = 0;
		return sets = rhs;
	}
}