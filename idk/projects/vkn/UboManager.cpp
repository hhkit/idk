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
		while (_buffers.size() > _curr_buffer_idx && !_buffers[_curr_buffer_idx].CanAdd(size))
		{
			++_curr_buffer_idx;
		}
		if (_buffers.size() <= _curr_buffer_idx)
		{
			auto&& [buffer, offset] = MakeBuffer();
			decltype(DataPair::data) buf{};
			buf.reserve(_chunk_size+_alignment);
			auto& tmp = _buffers.emplace_back(DataPair{ std::move(buffer),std::move(buf),offset,_chunk_size,MemoryCollator{index_span{0,_chunk_size}} });
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
	void UboManager::Update(vk::Buffer buffer, index_span range, string_view data)
	{
		for (auto& dp : this->_buffers)
		{
			if (dp.Buffer() == buffer)
			{
				memcpy_s(dp.data.data()+dp.initial_offset+range._begin, dp.data.size()-range._begin, data.data(),data.size());
				return;
			}
		}
	}
	void UboManager::Free(vk::Buffer buffer, uint32_t offset, uint32_t size)
	{
		//Find the datapair with the buffer
		for (auto& dp: this->_buffers)
		{
			if (dp.Buffer() == buffer)
			{
				dp.Free(offset, size);
				return;
			}
		}
		//free it.
	}
	void UboManager::UpdateAllBuffers()
	{
		for (auto& [buffer_idx,memory_idx] : _allocation_table)
		{
			auto& block = _memory_blocks[memory_idx];
			auto& memory = block.memory;
			auto& buffer = _buffers[buffer_idx];
			auto initial_offset = buffer.initial_offset;//InitialOffset(buffer.data.data(), _alignment);


			if (buffer.data.size())
			{
				auto dst_size = block.size ;
				auto src_size = buffer.data.size() - initial_offset;
				IDK_ASSERT(src_size <= dst_size);
				hlp::MapMemory(*view.Device(), *memory, /*buffer.initial_offset*/0, std::data(buffer.data) + initial_offset, dst_size, view.Dispatcher());
			}
		}
	}

	void UboManager::Clear()
	{
		for (auto& data_pair : _buffers)
		{
			data_pair.Reset();
		}
		_curr_buffer_idx = 0;
	}
	uint32_t SizeAlignmentOffset(size_t sz, size_t alignment)
	{
		return s_cast<uint32_t>(sz + ((sz % alignment) ? alignment - (sz % alignment) : 0));
	}
//#pragma optimize("",off)
	uint32_t aaaa2 = 0;
	bool UboManager::DataPair::CanAdd(size_t len) const
	{
		aaaa2++;
		if (data.size() > 65536)
			throw;
		return collator.can_allocate(Aligned(len, sz_alignment), alignment);
		return block_size >= 
			data.size() + AlignmentOffset() +
			SizeAlignmentOffset(len,sz_alignment);
	}

	size_t UboManager::DataPair::AlignmentOffset() const
	{
		auto mod = (r_cast<intptr_t>(data.data()) + data.size()) % alignment;
		return (mod) ? alignment - (mod) : 0;
	}

	void UboManager::DataPair::Align()
	{
		auto padding = std::min(block_size-data.size(),AlignmentOffset());
		data.append(padding, 0);
	}
	uint32_t aaaa = 0;
	bool operator==(index_span lhs, index_span rhs)
	{
		return lhs._begin == rhs._begin && lhs._end == rhs._end;
	}
	uint32_t UboManager::DataPair::Add(size_t len, const void* data_) 
	{
		if (collator.free_range == collator.full_range)
		{
			initial_offset = InitialOffset(data.data(), alignment);
		}

		auto opt = collator.allocate(Aligned(len, sz_alignment), alignment);
		aaaa++;
		if (!opt)
			throw;
		auto [unaligned_offset,aligned_offset] = *opt;
		//free the alignment stuff so that we just need to free the allocated block later
		if(aligned_offset - unaligned_offset!=0)
			collator.mark_freed({ unaligned_offset,aligned_offset });

		data.resize(block_size);
		if (len+ aligned_offset > 65536)
			throw;
		memcpy_s(data.data() +initial_offset+ aligned_offset, data.capacity()-(initial_offset + aligned_offset), data_, len);
		return static_cast<uint32_t>(aligned_offset);
	}

	void UboManager::DataPair::Free(uint32_t offset, size_t len)
	{
		collator.mark_freed(index_span{ offset,offset + len });
	}

	void UboManager::DataPair::Reset() 
	{
		data.resize(0); 
		collator.reset();
	}

}