#pragma once
#include  <idk.h>
#include <vulkan/vulkan.hpp>
#include <vkn/VulkanView.h>
namespace idk::vkn
{

	struct UboManager
	{
		using memory_idx_t = size_t;
		using buffer_idx_t = size_t;



		template<typename T>
		std::pair<vk::Buffer, uint32_t> Add(const T& data);
		void UpdateAllBuffers();
		void clear();

	private:
		struct DataPair
		{
			vk::UniqueBuffer buffer;
			string data;
			size_t offset{};
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
		constexpr static uint32_t              chunk_size = 1 << 16;
		size_t                                 memory_chunk_size = 1 << 20; //Replace this with the limit obtained from device.
		//Maybe replace with allocator
		vector<Memory>                         memory_blocks;

		hash_table<buffer_idx_t, memory_idx_t> allocation_table;
		vector<DataPair>                       buffers;
		size_t                                 curr_buffer_idx{};

		VulkanView& view;


		size_t AllocateAndBind(vk::Buffer& buffer);

		std::pair<vk::UniqueBuffer,size_t> make_buffer();

		DataPair& find_pair(size_t size);
	};
}
#include "UboManager.inl"