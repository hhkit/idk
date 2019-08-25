#include "pch.h"
#include "UboManager.h"
#include "BufferHelpers.h"
#include <string>

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
		uint32_t offset = s_cast<uint32_t>(*memory.allocate(chunk_size));
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
			decltype(DataPair::data) buf{};
			buf.reserve(chunk_size);
			auto& tmp = buffers.emplace_back(DataPair{ std::move(buffer),std::move(buf),offset });
			tmp.alignment = offset_alignment();
			tmp.sz_alignment = size_alignment();
			allocation_table.emplace(buffers.size() - 1,memory_blocks.size() - 1);
		}
		else if (!buffers[curr_buffer_idx].CanAdd(size))
		{
			++curr_buffer_idx;
		}
		return buffers[curr_buffer_idx];
	}

	UboManager::UboManager(VulkanView& view_) : view{view_}
	{
		_alignment = view.BufferOffsetAlignment();
	}

	inline uint32_t UboManager::offset_alignment() { return view.BufferOffsetAlignment(); }

	inline uint32_t UboManager::size_alignment() { return view.BufferSizeAlignment(); }


	uint32_t InitialOffset(const void* ptr, uint32_t alignment)
	{
		uint32_t mod = s_cast<uint32_t>((r_cast<intptr_t>(ptr) % alignment));
		return (mod) ? alignment - mod : 0;
	}
	void UboManager::UpdateAllBuffers()
	{
		for (auto& [buffer_idx,memory_idx] : allocation_table)
		{
			auto& memory = memory_blocks[memory_idx].memory;
			auto& buffer = buffers[buffer_idx];
			auto initial_offset = InitialOffset(buffer.data.data(), _alignment);
			hlp::MapMemory(*view.Device(),*memory,buffer.offset,std::data(buffer.data)+initial_offset,buffer.data.size()-initial_offset,view.Dispatcher() );
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
	uint32_t SizeAlignmentOffset(size_t sz, size_t alignment)
	{
		return s_cast<uint32_t>(sz + ((sz % alignment) ? alignment - (sz % alignment) : 0));
	}
	bool UboManager::DataPair::CanAdd(size_t len) const
	{
		return data.capacity() >= len + data.size() + AlignmentOffset() + SizeAlignmentOffset(data.size(),sz_alignment);
	}

	size_t UboManager::DataPair::AlignmentOffset() const
	{
		auto mod = (r_cast<intptr_t>(data.data()) + data.size()) % alignment;
		return (mod) ? alignment - (mod) : 0;
	}

	void UboManager::DataPair::Align()
	{
		data.append(AlignmentOffset(), 0);
	}
	uint32_t UboManager::DataPair::Add(size_t len, const void* data_) {
		Align();
		uint32_t result = s_cast<uint32_t>(data.size()) - InitialOffset(data.data(),alignment);
		len = SizeAlignmentOffset(len, sz_alignment);
		data.append(r_cast<const char*>(data_), len);
		return result;
	}

}