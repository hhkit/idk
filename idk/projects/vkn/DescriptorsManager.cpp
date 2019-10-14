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

	DescriptorSetLookup DescriptorsManager::Allocate(const hash_table < vk::DescriptorSetLayout, std::pair<uint32_t,std::array<uint32_t, DescriptorTypeI::size()>>>& allocations)
	{
		hash_table<vk::DescriptorSetLayout, DescriptorSets> result;
		auto& m_device = pools.view.Device();
		auto& dispatcher = pools.view.Dispatcher();
		vector<bool> allocated(allocations.size()*DescriptorTypeI::size(), false);
		//hash_table<vk::DescriptorType, uint32_t> num_req;
		bool req_more = false;
		std::array<uint32_t, DescriptorTypeI::size()> num_req{};
		do
		{
			req_more = false;
			//num_req.clear();
			uint32_t i = 0;
			for(auto& n : num_req)
				n = 0;//clear the previous iteration's numbers.

			for (auto& [layout, alloc_info] : allocations)
			{
				auto& [num_ds, des] = alloc_info;
				bool is_nonzero = false;
				for (auto& n : des)
				{
					is_nonzero |= static_cast<bool>(n);
				}
				if (!allocated[i] && is_nonzero)
				{
					auto pool = pools.TryGet(des);
					allocated[i] = static_cast<bool>(pool);
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
						for (size_t index = 0; index < std::size(num_req); ++index)
						{
							num_req[index] += des[index];
						}
						req_more = true;
					}
				}
				++i;
			}
			if(req_more)
				pools.Add(num_req._Elems);
		} while (req_more);
		return result;
	}
	DescriptorSetLookup  DescriptorsManager::Allocate(const hash_table<vk::DescriptorSetLayout, std::pair<vk::DescriptorType, uint32_t>>& allocations)
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
		hash_table<vk::DescriptorType,uint32_t> num_req;
		do
		{
			num_req.clear();
			uint32_t i = 0;
			for (auto& [layout, des] : allocations)
			{
				auto& [type, num_ds] = des;
				if (!allocated[i]&&num_ds>0)
				{
					auto pool = pools.TryGet(num_ds,type);
					allocated[i] = static_cast<bool>(pool);
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
						num_req[type] += num_ds;
					}
				}
				++i;
			}
			for (auto& [type, num] : num_req)
			{
				pools.Add(num,type);
			}
		} while (num_req.size());
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