#pragma once

#include <vector>
#include <array>
#include <atomic>

#include <machine.h>
#include <parallel/cache_aligned_wrapper.h>

namespace idk::mt
{
	// wraps a struct such that it is aligned to the cache line

	template<typename T, size_t MaxThds = 128>
	class HazardPointerDL
	{
	public:
		HazardPointerDL();
		~HazardPointerDL();
		
		T* protect(T* ptr, const int id);
		void clear(const int id);
		void free(T* ptr, T* curr_tail, const int id);
	private:
		static constexpr size_t free_threshold = 4; 
		
		std::array<cache_aligned_wrapper<std::atomic<T*>>, MaxThds> hazard_ptrs;
		std::array<cache_aligned_wrapper<std::vector<T*>>, MaxThds> retire_list;
	};
	
	template<typename T, size_t MaxThds>
	HazardPointerDL<T, MaxThds>::HazardPointerDL()
	{
		for (auto& elem : hazard_ptrs)
			elem.value.store(nullptr, std::memory_order_relaxed);
	}
	
	template<typename T, size_t MaxThds>
	HazardPointerDL<T, MaxThds>::~HazardPointerDL()
	{
		for (auto& elem : retire_list)
			for (auto& ptr : elem.value)
				delete ptr;
	}
	
	template<typename T, size_t MaxThds>
	T* HazardPointerDL<T, MaxThds>::protect(T* ptr, const int id)
	{
		hazard_ptrs[id].value.store(ptr);
		return ptr;
	}
	
	template<typename T, size_t MaxThds>
	void HazardPointerDL<T, MaxThds>::clear(const int id)
	{
		hazard_ptrs[id].value.store(nullptr, std::memory_order_release);
	}
	
	template<typename T, size_t MaxThds>
	void HazardPointerDL<T, MaxThds>::free(T* ptr, T* tail, const int id)
	{
		auto& curr_list = retire_list[id].value;
		for (auto& elem : curr_list)
			IDK_ASSERT(elem != ptr);
		curr_list.push_back(ptr);
		if (curr_list.size() < free_threshold)
			return;
			
		for (unsigned i = 0; i < curr_list.size();)
		{
			auto freeme = curr_list[i];
			if (freeme->next.load() == tail)
			{
				// can't delete the node before the current tail
				++i;
				continue;
			}
			bool canDelete = true;
			for (int thd = 0; thd < MaxThds; ++thd)
			{
				T* curr_hazard_ptr = hazard_ptrs[thd].value.load();
				// if we are trying to free a ptr that is
				// 	1. protected
				//	3. the next of a protected ptr 
				// we cannot free the ptr 
				
				if (curr_hazard_ptr == ptr || ptr->next.load() == curr_hazard_ptr)
				{
					canDelete = false;
					break;
				}
			}
			if (canDelete)
			{
				curr_list.erase(curr_list.begin() + i);
				delete freeme;
				continue;
			}
			++i;
		}
	}
}