#include "pch.h"
#include "MemoryAllocator.h"
#include <vkn/BufferHelpers.h>
#include <vkn/VulkanView.h>
#include <sstream>
namespace idk::vkn
{
	VulkanView& View();
}
namespace idk::vkn::hlp
{
	static hash_set<MemoryAllocator*> _allocators;
	std::pair<size_t,size_t> DumpAllocator(std::ostream& out,const MemoryAllocator& alloc)
	{
		size_t alloc_allocced = 0;
		size_t alloc_freed = 0;
		for (auto& mems : alloc.memories)
		{
			size_t chunk_alloced = 0;
			size_t chunk_free = 0;
			for (auto& mem : mems.second.memories)
			{
				size_t free = 0;
				for (auto& freed : mem.free_list)
				{
					free += freed.end - freed.start;
				}
				out << "\t\tSlice Allocated[" << (mem.sz - free) << "/" << mem.sz << "]\n";
				chunk_alloced += mem.sz;
				chunk_free += free;
			}
			out << "\tChunk Allocated[" << (chunk_alloced - chunk_free) << "/" << chunk_alloced << "]\n";
			alloc_allocced += chunk_alloced;
			alloc_freed += chunk_free;
		}
		out << " Allocator Allocated[" << (alloc_allocced - alloc_freed) << "/" << alloc_allocced << "]\n";
		return { alloc_allocced,alloc_freed };
	}
	string DumpAllocators()
	{
		std::stringstream out;
		size_t allocator_index=0;
		size_t total_alloc = 0;
		size_t total_free  =0;
		for (auto& palloc : _allocators)
		{
			auto& alloc = *palloc;
			out << "Allocator[" << allocator_index << "]{\n";
			auto [alloc_allocced, alloc_freed]=DumpAllocator(out,alloc);
			total_alloc+= alloc_allocced;
			total_free+= alloc_freed;
			//for (auto& block : alloc)
			//{
			//}
			out << "}\n";
		}
		out << "Total Allocated[" << (total_alloc- total_free) << "/" << total_alloc<< "]\n";
		return out.str();
	}
	using detail::Memories;

	MemoryAllocator::MemoryAllocator(vk::Device d, vk::PhysicalDevice pd) :device{ d }, pdevice{ pd }{_allocators.emplace(this); }
	MemoryAllocator::MemoryAllocator() : device{ *View().Device() }, pdevice{ View().PDevice() }{_allocators.emplace(this); }
	MemoryAllocator::UniqueAlloc MemoryAllocator::Allocate(vk::Device d, uint32_t mem_type, vk::MemoryRequirements mem_req)
	{
		auto itr = memories.find(mem_type);
		if (itr == memories.end())
		{
			itr = memories.emplace(mem_type, Memories{ d,mem_type }).first;
		}
		auto& mem = itr->second;
		auto&& [mem_idx, offsets] = mem.Allocate(mem_req.size,mem_req.alignment);
		auto& [u_offset, offset] = offsets;
		return std::make_unique<Alloc>(mem, mem_idx, u_offset,offset, mem_req.size);
	}
	MemoryAllocator::UniqueAlloc MemoryAllocator::Allocate(vk::PhysicalDevice pd, vk::Device d, vk::Buffer& buffer, vk::MemoryPropertyFlags prop)
	{
		auto mem_req = d.getBufferMemoryRequirements(buffer, vk::DispatchLoaderDefault{});
		auto mem_type = hlp::findMemoryType(pd, mem_req.memoryTypeBits, prop);
		return Allocate(d, mem_type, mem_req);
	}
	MemoryAllocator::~MemoryAllocator()
	{
		_allocators.erase(this);
	}
	UniqueAlloc MemoryAllocator::Allocate(vk::Image image, vk::MemoryPropertyFlags prop)
	{
		vk::MemoryRequirements memRequirements = device.getImageMemoryRequirements(image);
		auto mem_type = hlp::findMemoryType(pdevice, memRequirements.memoryTypeBits, prop);
		
		return Allocate(device, mem_type, memRequirements);
	}
	MemoryAllocator::UniqueAlloc  MemoryAllocator::Allocate(vk::Buffer& buffer, vk::MemoryPropertyFlags prop)
	{
		return Allocate(pdevice, device, buffer, prop);
	}

	vk::DeviceMemory MemoryAllocator::Alloc::Control::Memory() const { return *IntMemory().memory; }

	Memories::Memory& MemoryAllocator::Alloc::Control::IntMemory() const { return src->memories[index]; }

	MemoryAllocator::Alloc::Control::~Control()
	{
		if (src)
			IntMemory().Free(unaligned_offset, size+(offset- unaligned_offset));
	}

	MemoryAllocator::Alloc::~Alloc() = default;

//// 
	void Memories::Memory::Free(size_t offset, size_t size)
	{
		MemoryRange range{ MemoryRange{offset, offset + size} };
		//TODO use some kind of find closest (maybe upper bound)?
		for (auto& freed : free_list)
		{
			if (freed.overlaps(range))
			{
				auto new_range = freed;
				free_list.erase(freed);
				new_range.absorb(range);
				auto itr = free_list.lower_bound(new_range);
				if (itr!=free_list.end()&&itr->overlaps(new_range))
				{
					new_range.absorb(*itr);
					free_list.erase(itr);
				}
				range = new_range;
				break;
			}
		}
		free_list.emplace(range);
	}

	size_t Aligned(size_t offset, size_t alignment)
	{
		size_t mod = (offset) % alignment;
		return offset + ((mod)?(alignment - mod):0);
	}

	//Returns unaligned and aligned offset if it is allocated
	std::optional<std::pair<size_t,size_t>> Memories::Memory::Allocate(size_t size,size_t alignment)
	{
		std::optional<std::pair<size_t, size_t>> result{};
		for (auto& range : free_list)
		{
			if (range.can_split(size, alignment))
			{
				auto new_range = range;
				auto result_range = new_range.split(size, alignment);
				free_list.erase(range);
				if (new_range)
					free_list.emplace(new_range);
				result = { result_range->start,Aligned(result_range->start,alignment) };
				break;
			}
		}
		if (!result)
		{
			auto unaligned_offset = curr_offset;
			auto aligned_offset = Aligned(curr_offset, alignment);
			if (sz >= aligned_offset + size)
			{
				result = { unaligned_offset,aligned_offset };
				curr_offset = aligned_offset+size;
			}

		}
		return result;
	}

	Memories::Memories(
		vk::Device d,
		uint32_t mem_type,
		size_t chunkSize
	) : device{ d },
		type{ mem_type },
		chunk_size{ chunkSize }{}
	Memories::Memory& Memories::Add(size_t min)
	{
		auto sz = std::max(chunk_size, min);
		LOG_TO(LogPool::GFX, "Allocating %ull", sz);
		memories.emplace_back(device.allocateMemoryUnique(
			vk::MemoryAllocateInfo{
				Track(sz),type
			}, nullptr, vk::DispatchLoaderDefault{}
		), sz);

		return memories.back();
	}

	std::pair<uint32_t, std::pair<size_t, size_t>> detail::Memories::Allocate(size_t size, size_t alignment)
	{
		std::optional<std::pair<size_t,size_t>> alloc_offset;
		uint32_t index{};
		for (auto& mem : memories)
		{
			alloc_offset = mem.Allocate(size, alignment);
			if (alloc_offset)
			{
				break;
			}
			++index;
		}
		if (!alloc_offset)
		{
			auto& mem = Add(size);
			alloc_offset = mem.Allocate(size, alignment);
		}
		return std::make_pair(index, *alloc_offset);
	}


	//returns true if absorbed (overlapping)

	bool detail::Memories::Memory::MemoryRange::overlaps(const MemoryRange& range) const noexcept
	{
		return !(start > range.end || end < range.start);
	}

	bool detail::Memories::Memory::MemoryRange::absorb(const MemoryRange& range) noexcept
	{
		bool should_absorb = overlaps(range);
		start = (should_absorb) ? std::min(range.start, start) : start;
		end = (should_absorb) ? std::max(range.end, end) : end;
		return should_absorb;
	}

	bool detail::Memories::Memory::MemoryRange::can_split(size_t sz, size_t align) const noexcept
	{
		auto aligned_start = Aligned(start, align);
		auto new_start = aligned_start + sz;
		return (end >= new_start);
	}

	std::optional<detail::Memories::Memory::MemoryRange> detail::Memories::Memory::MemoryRange::split(size_t sz, size_t align) noexcept
	{
		std::optional<detail::Memories::Memory::MemoryRange> result{ };
		auto aligned_start = Aligned(start, align);
		auto new_start = aligned_start + sz;
		if (end >= new_start)
			result = MemoryRange{ start, new_start - start };
		start = aligned_start + sz;
		return result;
	}
	detail::Memories::Memory::MemoryRange::operator bool()const noexcept
	{
		return start > end;
	}

}
