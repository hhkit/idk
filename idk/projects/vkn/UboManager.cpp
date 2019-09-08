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
		if (!_memory_blocks.size() || _memory_blocks.back().can_allocate(_chunk_size))
		{
			_memory_blocks.emplace_back(view, buffer, _memory_chunk_size);
		}
		auto& memory = _memory_blocks.back();
		uint32_t offset = s_cast<uint32_t>(*memory.allocate(_chunk_size));
		hlp::BindBufferMemory(*view.Device(), buffer, *memory.memory, offset, view.Dispatcher());
		return offset;
	}

	std::pair<vk::UniqueBuffer,size_t> UboManager::MakeBuffer()
	{
		auto buffer = hlp::CreateBuffer(*view.Device(), _chunk_size, vk::BufferUsageFlagBits::eUniformBuffer, view.Dispatcher());
		;
		return std::make_pair( std::move(buffer),AllocateAndBind(*buffer));
	}

	UboManager::DataPair& UboManager::FindPair(size_t size)
	{
		if (_buffers.size() > _curr_buffer_idx && !_buffers[_curr_buffer_idx].CanAdd(size))
		{
			++_curr_buffer_idx;
		}
		if (_buffers.size() <= _curr_buffer_idx)
		{
			auto&& [buffer, offset] = MakeBuffer();
			decltype(DataPair::data) buf{};
			buf.reserve(_chunk_size);
			auto& tmp = _buffers.emplace_back(DataPair{ std::move(buffer),std::move(buf),offset });
			tmp.alignment = OffsetAlignment();
			tmp.sz_alignment = SizeAlignment();
			_allocation_table.emplace(_buffers.size() - 1,_memory_blocks.size() - 1);
		}
		return _buffers[_curr_buffer_idx];
	}

	UboManager::UboManager(VulkanView& view_) : view{view_}
	{
		_alignment = view.BufferOffsetAlignment();
	}

	uint32_t UboManager::OffsetAlignment()const { return view.BufferOffsetAlignment(); }

	uint32_t UboManager::SizeAlignment() const{ return view.BufferSizeAlignment(); }


	uint32_t InitialOffset(const void* ptr, uint32_t alignment)
	{
		uint32_t mod = s_cast<uint32_t>((r_cast<intptr_t>(ptr) % alignment));
		return (mod) ? alignment - mod : 0;
	}
	void UboManager::UpdateAllBuffers()
	{
		for (auto& [buffer_idx,memory_idx] : _allocation_table)
		{
			auto& memory = _memory_blocks[memory_idx].memory;
			auto& buffer = _buffers[buffer_idx];
			auto initial_offset = InitialOffset(buffer.data.data(), _alignment);
			hlp::MapMemory(*view.Device(),*memory,buffer.offset,std::data(buffer.data)+initial_offset,buffer.data.size()-initial_offset,view.Dispatcher() );
		}
	}

	void UboManager::Clear()
	{
		for (auto& data_pair : _buffers)
		{
			data_pair.data.resize(0);
		}
		_curr_buffer_idx = 0;
	}
	uint32_t SizeAlignmentOffset(size_t sz, size_t alignment)
	{
		return s_cast<uint32_t>(sz + ((sz % alignment) ? alignment - (sz % alignment) : 0));
	}
	bool UboManager::DataPair::CanAdd(size_t len) const
	{
		return data.capacity() >= len +AlignmentOffset() + SizeAlignmentOffset(data.size(),sz_alignment);
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