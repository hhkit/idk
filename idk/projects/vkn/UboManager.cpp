#include "pch.h"
#include "UboManager.h"
#include "BufferHelpers.h"

namespace idk::vkn
{

	UboManager::Memory::Memory(VulkanView& view, vk::Buffer& buffer, size_t capacity_) :capacity{ capacity_ }
	{
		auto req = view.Device()->getBufferMemoryRequirements(buffer, view.Dispatcher());
		memory = hlp::AllocateBuffer(
			view.PDevice(), *view.Device(), buffer,
			vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
			view.Dispatcher()
		);
	}

	bool UboManager::Memory::can_allocate(size_t chunk) const {
		return(size + chunk <= capacity);
	}

	//Returns the offset to start from

	std::optional<size_t> UboManager::Memory::allocate(size_t chunk)
	{
		std::optional<size_t>result{};
		if (can_allocate(chunk))
		{
			result = size;
			size += chunk;
		}
		return result;
	}

	size_t UboManager::AllocateAndBind(vk::Buffer& buffer)
	{
		if (!memory_blocks.size() || memory_blocks.back().can_allocate(chunk_size))
		{
			memory_blocks.emplace_back(view, buffer, memory_chunk_size);
		}
		auto& memory = memory_blocks.back();
		size_t offset = *memory.allocate(chunk_size);
		hlp::BindBufferMemory(*view.Device(), buffer, *memory.memory, offset, view.Dispatcher());
		return offset;
	}

	std::pair<vk::UniqueBuffer,size_t> UboManager::make_buffer()
	{
		auto buffer = hlp::CreateBuffer(*view.Device(), chunk_size, vk::BufferUsageFlagBits::eUniformBuffer, view.Dispatcher());
		;
		return std::make_pair( std::move(buffer),AllocateAndBind(*buffer));
	}

	UboManager::DataPair& UboManager::find_pair(size_t size)
	{
		if (buffers.size() <= curr_buffer_idx)
		{
			auto&& [buffer, offset] = make_buffer();
			buffers.emplace_back(DataPair{ std::move(buffer),string(chunk_size,'\0'),offset });
			allocation_table.emplace(buffers.size() - 1,memory_blocks.size() - 1);
		}
		else if (!buffers[curr_buffer_idx].CanAdd(size))
		{
			++curr_buffer_idx;
		}
		return buffers[curr_buffer_idx];
	}

	void UboManager::UpdateAllBuffers()
	{
		for (auto& [buffer_idx,memory_idx] : allocation_table)
		{
			auto& memory = memory_blocks[memory_idx].memory;
			auto& buffer = buffers[buffer_idx];
			hlp::MapMemory(*view.Device(),*memory,buffer.offset,std::data(buffer.data),buffer.data.size(),view.Dispatcher() );
		}
	}

	void UboManager::clear()
	{
		for (auto& data_pair : buffers)
		{
			data_pair.data.resize(0);
		}
		curr_buffer_idx = 0;
	}

	bool UboManager::DataPair::CanAdd(size_t len) const
	{
		return data.capacity() >= len + data.size() + AlignmentOffset();
	}

	size_t UboManager::DataPair::AlignmentOffset() const
	{
		return (data.size() % 16) ? 16 - (data.size() % 16) : 0;
	}

	void UboManager::DataPair::Align()
	{
		data.append(AlignmentOffset(), 0);
	}

	uint32_t UboManager::DataPair::Add(size_t len, const void* data_) {
		uint32_t result = s_cast<uint32_t>(data.size());
		Align();
		data.append(r_cast<const char*>(data_), len);
		return result;
	}

}