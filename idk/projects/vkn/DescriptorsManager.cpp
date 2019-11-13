#include "pch.h"
#include "DescriptorsManager.h"
#include <vkn/BufferHelpers.h>
#include <vkn/VulkanState.h>
#include <vkn/VulkanView.h>
#include <vkn/DescriptorCountArray.h>
//hash_table<vk::DescriptorSetLayout, DescriptorSetManager> ds_sets;

namespace idk::vkn
{

#pragma region allocation helpers
	struct AllocationTracker
	{
		using DsCountArray = std::array<uint32_t, DescriptorTypeI::size()>;
		bool req = false;
		DsCountArray  allocation_queue;

		void QueueForAllocation(const DsCountArray& alloc_request)
		{
			for (size_t i = 0; i < std::size(allocation_queue); ++i)
			{
				allocation_queue[i] += alloc_request[i];
			}
			req = true;
		}

		void QueueForAllocation(const std::pair<vk::DescriptorType, uint32_t>& alloc_request)
		{
			DsCountArray arr{};
			arr[DescriptorTypeI::map(alloc_request.first)] = alloc_request.second;
			QueueForAllocation(arr);
		}
		void AllocatePools(DescriptorPoolsManager& pools)
		{
			if (req)
				pools.Add(allocation_queue._Elems);
		}
	};

	struct LayoutAllocationInfo
	{
		vector<vk::DescriptorSetLayout> layouts;
		vector<std::pair<size_t, size_t>> ranges;
		void insert(uint32_t num, vk::DescriptorSetLayout layout)
		{
			ranges.emplace_back(layouts.size(), layouts.size() + num);
			layouts.insert(layouts.end(), num, layout);
		}
	};

#pragma endregion


	DescriptorsManager::DescriptorsManager(VulkanView& view) :pools{view}
	{
	}
	DescriptorSetLookup  DescriptorsManager::Allocate(const hash_table < vk::DescriptorSetLayout, std::pair<uint32_t, DsCountArray>>& allocations)
	{
		DescriptorSetLookup result;
		//Checks to see if the there are enough pre-allocated DSes, if not, allocate from pool.
		Grow(allocations);
		for (auto& [layout, reqs] : allocations)
		{
			auto itr = free_dses.find(layout);
			auto opt = itr->second.GetRange(reqs.first);
			//Grow should have ensured that we have the capacity.
			IDK_ASSERT(opt);
			if (opt)
				result[layout] = std::move(*opt);
			else throw "AAAA";
		}
		return result;
	}


	DescriptorSetLookup  DescriptorsManager::Allocate(const hash_table<vk::DescriptorSetLayout, std::pair<vk::DescriptorType, uint32_t>>& allocations)
	{
		DescriptorSetLookup result;
		//Checks to see if the there are enough pre-allocated DSes, if not, allocate from pool.
		Grow(allocations);
		for (auto& [layout, reqs] : allocations)
		{
			auto itr = free_dses.find(layout);
			auto opt = itr->second.GetRange(reqs.second);
			//Grow should have ensured that we have the capacity.
			IDK_ASSERT(opt);
			result[layout] = *opt;
		}
		return result;
	}


	void DescriptorsManager::Grow(const hash_table<vk::DescriptorSetLayout, std::pair<vk::DescriptorType, uint32_t>>& allocations)
	{
		using pair_t =std::pair<uint32_t, DsCountArray>;
		hash_table<vk::DescriptorSetLayout, pair_t> conv{};
		for (auto& [layout, req] : allocations)
		{
			DsCountArray arr{};
			arr[DescriptorTypeI::map(req.first)] = 1;
			conv[layout] = pair_t{req.second,arr};
		}
		Grow(conv);
	}
#pragma optimize("",off)
	void DescriptorsManager::Grow(const hash_table<vk::DescriptorSetLayout, std::pair<uint32_t, DsCountArray>>& allocations)
	{
		//Redo.
		AllocationTracker tracker{};
		auto& m_device = pools.view.Device();
		auto& dispatcher = pools.view.Dispatcher();
		//Figure how many new allocations we need to make
		for (auto& [layout, req] : allocations)
		{
			auto itr = free_dses.find(layout);
			auto num_ds = req.first;
			if (itr == free_dses.end()||itr->second.num_available() < num_ds)
			{
				auto diff = num_ds;
				tracker.QueueForAllocation(req.second*diff);
			}
		}
		//Allocate the pools required.
		tracker.AllocatePools(pools);

		bool failed = false;
		hash_table<vk::DescriptorPool, LayoutAllocationInfo> layouts{};
		//Prepare the request
		for (auto& [layout, req] : allocations)
		{
			auto itr = free_dses.find(layout);
			auto num_ds = req.first;
			if (itr == free_dses.end() || itr->second.num_available() < num_ds)
			{
				auto diff = (itr != free_dses.end()) ? num_ds - itr->second.num_available() : num_ds;
				auto pool = pools.TryGet(req.second* diff);
				
				//TODO compute num_ds with layout's number of descriptors
				if (pool)
				{
					auto dbg_size = layouts[*pool].layouts.size();
					layouts[*pool].insert(diff, layout);
				}
				else
					failed = true;
			}
		}
		for (auto& [pool, alloc_info] : layouts)
		{
			//Request all at once.
			vk::DescriptorSetAllocateInfo allocInfo
			{
				pool
				,hlp::arr_count(alloc_info.layouts)
				,std::data(alloc_info.layouts)
			};
			//This call was apparently expensive?
			auto alloc_chunk = m_device->allocateDescriptorSets(allocInfo, dispatcher);
			for (auto& range : alloc_info.ranges)
			{
				auto layout = alloc_info.layouts[range.first];
				auto& curr_vec = free_dses[layout].sets;
				//Insert the new descriptor sets into the respective free list
				curr_vec.insert(curr_vec.end(), alloc_chunk.begin() + range.first, alloc_chunk.begin() + range.second);
			}
		}
	}

	/*


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
					auto des_copy = des;
					for (auto& d : des_copy)
					{
						d *= num_ds;
					}
					auto pool = pools.TryGet(des_copy);
					allocated[i] = static_cast<bool>(pool);
					if (pool)
					{
						//TODO compute num_ds with layout's number of descriptors
						vector<vk::DescriptorSetLayout> layouts{ num_ds, layout };
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
							num_req[index] += des_copy[index];
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
	*/
	void DescriptorsManager::Reset()
	{
		for (auto& dss : free_dses)
		{
			dss.second.curr_index = 0;
		}
		//pools.ResetAll();
	}

	uint32_t DescriptorSets::size() const
	{
		return s_cast<uint32_t>(sets.size());
	}

	uint32_t DescriptorSets::num_available() const
	{
		return size()-curr_index;
	}

	vk::DescriptorSet& DescriptorSets::GetNext()
	{
		return sets[curr_index++];
	}

	std::optional<DescriptorSets> DescriptorSets::GetRange(uint32_t num)
	{
		std::optional<DescriptorSets> result{};
		if (num + curr_index <= size())
		{
			auto curr_itr = sets.begin() + curr_index;
			result = DescriptorSets{};
			auto& set = result->sets;
			set.insert(set.end(),curr_itr, curr_itr + num);
			curr_index += num;
		}
		return result;
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