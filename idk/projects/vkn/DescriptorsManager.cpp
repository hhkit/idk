#include "pch.h"
#include "DescriptorsManager.h"
#include <vkn/DescriptorPoolsManager.inl>
#include <vkn/BufferHelpers.h>
#include <vkn/VulkanState.h>
#include <vkn/VulkanView.h>
#include <vkn/DescriptorCountArray.h>
#include <ds/index_span.inl>

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
			last_use[layout] = std::chrono::high_resolution_clock::now();
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
			last_use[layout] = std::chrono::high_resolution_clock::now();
		}
		return result;
	}

	void DescriptorsManager::Free(vk::DescriptorSetLayout layout, vk::DescriptorSet ds)
	{
		//auto itr = free_dses.find(layout);
		//itr->second.FreeDS(ds);
		free_dses[layout].FreeDS(ds);
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
//// 
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
				auto& dss = free_dses[layout];
				auto& src_pools = dss.src_pools;
				auto& curr_vec = dss.sets;
				auto& alloc_vec = dss.alloced_sets;
				//Insert the new descriptor sets into the respective free list
				src_pools.emplace_back(index_span{ curr_vec.size(),curr_vec.size() + range.second - range.first }, pool);
				curr_vec.insert(curr_vec.end(), alloc_chunk.begin() + range.first, alloc_chunk.begin() + range.second);
				alloc_vec.insert(alloc_vec.end(), alloc_chunk.begin() + range.first, alloc_chunk.begin() + range.second);

			}
		}
	}

	void DescriptorsManager::Cull()
	{/*
		auto now = std::chrono::high_resolution_clock::now();
		for (auto itr = last_use.begin();itr!=last_use.end();)
		{
			auto& [layout, time] = *itr;
			auto next = itr;
			++next;
			if ( (now-time) > std::chrono::milliseconds{ 300 })
			{
				auto fitr = free_dses.find(layout);
				if (fitr != free_dses.end())
				{
					free_dses.erase(fitr);
					next = last_use.erase(itr);
				}
			}
			itr = next;

		}*/
		auto itr = free_dses.begin();
		while (itr != free_dses.end())
		{
			auto next = itr;
			next++;
			if (!itr->second.size())
				next = free_dses.erase(itr);
			itr = next;
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

	vk::DescriptorSet DescriptorSets::GetNext()
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
	void DescriptorSets::FreeDS(vk::DescriptorSet ds)
	{
		for (size_t i = 0; i < sets.size(); i++)
		{
			if (ds == sets[i])
			{
				if(curr_index>0)
					std::swap(sets[curr_index - 1], sets[i]);
				curr_index--;
				break;
			}
		}
		if (curr_index == 0)
		{
			for (auto& [range, pool] : src_pools)
			{
				vk::ArrayProxy<const vk::DescriptorSet> ap( static_cast<uint32_t>(range.size()),range.to_span(alloced_sets).data());
				View().Device()->freeDescriptorSets(pool,ap);
			}
			sets.clear();
			alloced_sets.clear();
			src_pools.clear();
			curr_index = 0;
			//View().Device()->freeDescriptorSets()
		}
	}
}