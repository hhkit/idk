#pragma once
#include  <idk.h>
#include <vulkan/vulkan.hpp>
#include <vkn/VulkanView.h>
#include <meta/stl_hack.h>
namespace idk::vkn
{
	template<typename T>
	using tallocator = std::allocator<T>;
	struct UboManager
	{
		using memory_idx_t = size_t;
		using buffer_idx_t = size_t;


		UboManager(VulkanView& view);

		uint32_t OffsetAlignment()const;
		uint32_t SizeAlignment()  const;

		template<typename T>
		std::pair<vk::Buffer, uint32_t> Add(const T& data);
		void UpdateAllBuffers();
		void Clear();

	private:
		struct DataPair
		{
			vk::UniqueBuffer buffer{};
			std::basic_string<char,std::char_traits<char>,tallocator<char>> data{};
			size_t offset{};
			uint32_t alignment{};
			uint32_t sz_alignment{};
			//DataPair() = default;
			//DataPair(DataPair&&) noexcept = default;
			//DataPair(const DataPair&) = delete;
			bool CanAdd(size_t len)const;
			size_t AlignmentOffset()const;
			void Align();
			uint32_t Add(size_t len, const void* data_);
			vk::Buffer& Buffer() { return *buffer; }

			void Reset() { data.resize(0); }
		};
		struct Memory
		{
			vk::UniqueDeviceMemory memory;
			size_t size{}, capacity{};
			Memory(VulkanView& view, vk::Buffer& buffer, size_t capacity_);
			bool can_allocate(size_t chunk)const;
			//Returns the offset to start from
			std::optional<size_t> allocate(size_t chunk);
		};
		VulkanView& view;
		uint32_t                               _alignment = 0x16;
		constexpr static uint32_t              _chunk_size = 1 << 16;
		size_t                                 _memory_chunk_size = 1 << 20; //Replace this with the limit obtained from device.
		//Maybe replace with allocator
		vector<Memory>                         _memory_blocks;

		hash_table<buffer_idx_t, memory_idx_t> _allocation_table;
		vector<DataPair>                       _buffers;
		size_t                                 _curr_buffer_idx{};



		size_t AllocateAndBind(vk::Buffer& buffer);

		std::pair<vk::UniqueBuffer,size_t> MakeBuffer();

		DataPair& FindPair(size_t size);
	};
	/*
	template<typename T>
	struct test_allocator
	{
		using const_pointer = const T*;
		using difference_type = ptrdiff_t;
		using pointer = T *;
		using size_type = size_t;
		using value_type = T;

		template<typename U>
		struct rebind
		{
			using other = test_allocator<U>;
		};
		template<typename ...Args>
		test_allocator(Args&& ...) noexcept {}

		static 	T* allocate(size_t sz)
		{
			return new T[sz + 32] + 11;

		}
		//const test_allocator& allocator()const { return *this; }

		static void deallocate(T* ptr, size_t)
		{
			delete[](ptr - 11);
		}
	};*/
}
MARK_NON_COPY_CTORABLE(idk::vkn::UboManager::DataPair)
MARK_NON_COPY_CTORABLE(idk::vkn::UboManager)
