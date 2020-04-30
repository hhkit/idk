#include "pch.h"
#include "MemoryAllocator.h"
#include <vkn/BufferHelpers.h>
#include <vkn/VulkanView.h>
#include <sstream>

#include <ds/index_span.inl>
#include <vkn/MemoryCollator.h>
namespace idk::vkn
{
	VulkanView& View();
	namespace dbg
	{
		string num_bytes_to_str(size_t num_bytes);
	}
}
namespace idk::vkn::hlp
{

	struct detail::Memories::Memory
	{
		Memory(vk::UniqueDeviceMemory&& mem, size_t size) :memory{ std::move(mem) }, collator{ {0,size} }{}

		void Free(size_t offset, size_t size);
		//Returns offset if it is allocated
		std::optional<std::pair<unaligned_t, aligned_t>> Allocate(size_t size, size_t alignment);

		vk::UniqueDeviceMemory memory;
		size_t sz()const noexcept;
		MemoryCollator collator;


	};

	using dbg::num_bytes_to_str;
	static hash_set<MemoryAllocator*> _allocators;
	std::pair<size_t,size_t> DumpAllocator(std::ostream& out,const MemoryAllocator& alloc)
	{
		size_t alloc_allocced = 0;
		size_t alloc_freed = 0;
		for (auto& mems : alloc.memories)
		{
			size_t chunk_alloced = 0;
			size_t chunk_free = 0;
			out << "\t\tMemory Type[" << mems.first << "]\n";
			for (auto& mem : mems.second.memories)
			{
				size_t free = 0;
				for (auto&& freed : mem.collator.free_list)
				{
					free += freed.size();
				}
				out << "\t\tFree Chunks[" << num_bytes_to_str(mem.collator.free_list.size())<< "]\n";
				out << "\t\tSlice Allocated[" << num_bytes_to_str(mem.sz() - free) << "/" << num_bytes_to_str(mem.sz()) << "]\n";
				chunk_alloced += mem.sz();
				chunk_free += free;
			}
			out << "\tChunk Allocated[" << num_bytes_to_str(chunk_alloced - chunk_free) << "/" << num_bytes_to_str(chunk_alloced) << "]\n";
			alloc_allocced += chunk_alloced;
			alloc_freed += chunk_free;
		}
		out << " Allocator Allocated[" << num_bytes_to_str(alloc_allocced - alloc_freed) << "/" << num_bytes_to_str(alloc_allocced) << "]\n";
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
		out << "Total Allocated[" << num_bytes_to_str(total_alloc- total_free) << "/" << num_bytes_to_str(total_alloc)<< "]\n";
		return out.str();
	}
	using detail::Memories;


//#pragma optimize("",off)

	static size_t dbg_threshold = 100000000;

	MemoryAllocator::MemoryAllocator(vk::Device d, vk::PhysicalDevice pd) :device{ d }, pdevice{ pd }{_allocators.emplace(this); }
	MemoryAllocator::MemoryAllocator() : device{ *View().Device() }, pdevice{ View().PDevice() }{_allocators.emplace(this); }
	MemoryAllocator::UniqueAlloc MemoryAllocator::Allocate(vk::Device d, uint32_t mem_type, vk::MemoryRequirements mem_req)
	{
		//lock.Lock();
		SimpleLockGuard guard{ lock };
		auto itr = memories.find(mem_type);
		if (itr == memories.end())
		{
			itr = memories.emplace(mem_type, Memories{ d,mem_type }).first;
		}
		auto& mem = itr->second;
		auto&& [mem_idx, offsets] = mem.Allocate(mem_req.size,mem_req.alignment);
		//if (mem_req.size > dbg_threshold)
		//	DebugBreak();
		//lock.Unlock();
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
		{
			IntMemory().Free(unaligned_offset, size + (offset - unaligned_offset));
			//while (src->memories.size() && src->memories.back().curr_offset == 0)
			//	src->memories.pop_back();
		}
		else
		{
			LOG_TO(LogPool::GFX, "Attempting to free weird shit");
		}
	}

	size_t MemoryAllocator::Alloc::BlockSize() const { return control.IntMemory().sz(); }

	MemoryAllocator::Alloc::~Alloc() = default;

//// 
	void Memories::Memory::Free(size_t offset, size_t size)
	{
		collator.mark_freed(index_span{ offset,offset + size });
	}


	//Returns unaligned and aligned offset if it is allocated
	std::optional<std::pair<detail::unaligned_t, detail::aligned_t>> Memories::Memory::Allocate(size_t size,size_t alignment)
	{
		return collator.allocate(size,alignment);
	}

	size_t detail::Memories::Memory::sz() const noexcept
	{
		return collator.full_range.size();
	}

	Memories::Memories(
		vk::Device d,
		uint32_t mem_type,
		size_t chunkSize
	) : device{ d },
		type{ mem_type },
		chunk_size{ chunkSize }{}
	detail::Memories::Memories(Memories&&) noexcept = default;
	Memories& detail::Memories::operator=(Memories&&)noexcept = default;
	detail::Memories::~Memories() = default;
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

	std::pair<uint32_t, std::pair<detail::unaligned_t, detail::aligned_t>> detail::Memories::Allocate(size_t size, size_t alignment)
	{
		std::optional<std::pair<unaligned_t, aligned_t>> alloc_offset;
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
			if (!alloc_offset)
				throw std::runtime_error("Failed to allocate graphics memory");
		}
		return std::make_pair(index, *alloc_offset);
	}


	//returns true if absorbed (overlapping)

}
