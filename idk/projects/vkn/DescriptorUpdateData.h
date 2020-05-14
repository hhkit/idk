#pragma once
#include <idk.h>
#include <vulkan/vulkan.hpp>
#include <forward_list>
#include <idk/memory/ArenaAllocator.inl>
#include <vkn/FreeList.h>
#include <vkn/FixedArenaAllocator.h>

namespace idk::vkn
{
	

	struct DescriptorUpdateData
	{
		static void* derp(void* ptr);
		template<typename T>
		static T* test_fa(T* ptr)
		{
			return reinterpret_cast<T*>(derp(ptr));
		}

		static void JustBreak();

		template<typename T>
		using allocator_t = FixedArenaAllocator<T>;
		template<typename T,size_t N>
		using allocator_n_t = FixedArenaAllocator<T,N>;
		template<typename T>
		using vector_a = std::vector<T, allocator_t<T>>;
		template<typename T>
		using fwd_list_a = std::forward_list<T, PoolAllocator<T >> ;
		allocator_t<char> alloc{};
		allocator_n_t<char, 1 <<20> list_alloc{};
		allocator_n_t<char,1<<20> list_alloc2{};

		vector_a<vk::WriteDescriptorSet> descriptorWrite{ alloc };
		fwd_list_a<vector_a<vk::DescriptorImageInfo>> image_infos{};// list_alloc};
		fwd_list_a<vk::DescriptorBufferInfo> buffer_infos{};// list_alloc2 };

		vector_a<vk::WriteDescriptorSet> scratch_descriptorWrite{ alloc };
		vector_a<vector_a<vk::DescriptorImageInfo>> scratch_image_info{ alloc };
		fwd_list_a<vk::DescriptorBufferInfo> scratch_buffer_infos{};// list_alloc2};


		void AbsorbFromScratch();
		void SendUpdates();
		void Reset();
	};
}