#include "pch.h"
#include "MemoryAllocator.h"
#include <vkn/BufferHelpers.h>
namespace idk::vkn::hlp
{
	MemoryAllocator::UniqueAlloc MemoryAllocator::Allocate(vk::PhysicalDevice pd, vk::Device d, vk::Buffer& buffer, vk::MemoryPropertyFlags prop)
	{
		auto mem_req = d.getBufferMemoryRequirements(buffer, vk::DispatchLoaderDefault{});
		auto mem_type = hlp::findMemoryType(pd, mem_req.memoryTypeBits, prop);
		auto itr = memories.find(mem_type);
		if (itr == memories.end())
		{
			itr = memories.emplace(mem_type, Memories{ d,mem_type }).first;
		}
		auto& mem = itr->second;
		auto&& [mem_idx, offset] = mem.Allocate(mem_req.size);
		return std::make_unique<Alloc>(mem, mem_idx, offset, mem_req.size);
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
			IntMemory().Free(offset, size);
	}

	MemoryAllocator::Alloc::~Alloc() = default;

	void Memories::Memory::Free(size_t offset, size_t size)
	{
		//TODO actually manage the freed memory
	}

	//Returns offset if it is allocated

	std::optional<size_t> Memories::Memory::Allocate(size_t size)
	{
		std::optional<size_t> result{};
		if (sz >= curr_offset + size)
		{
			result = curr_offset;
			curr_offset += size;
		}
		return result;
	}

}