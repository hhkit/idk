#pragma once
#include <idk.h>
#include <vulkan/vulkan.hpp>

namespace idk::vkn::hlp
{
	struct Memories
	{
		struct Memory
		{
			vk::DeviceMemory memory;
			size_t sz{};
			size_t curr_offset{};
			void Free(size_t offset, size_t size);
			//Returns offset if it is allocated
			std::optional<size_t> Allocate(size_t size);
		};
		vector<Memory> memories;
		Memory& Add()
		{
			//TODO actually create the memory
			return memories.back();
		}
		std::pair<uint32_t, size_t> Allocate(size_t size)
		{
			std::optional<size_t> alloc_offset;
			uint32_t index{};
			for (auto& mem : memories)
			{
				alloc_offset = mem.Allocate(size);
				if (alloc_offset)
				{
					break;
				}
				++index;
			}
			if (!alloc_offset)
			{
				auto& mem = Add();
				alloc_offset = mem.Allocate(size);
				if (alloc_offset)
				{
					++index;
				}
			}
			return std::make_pair(index, *alloc_offset);
		}
	};
	class MemoryAllocator
	{
	public:
		class Alloc
		{
		public:
			vk::DeviceMemory Memory()const { return control.Memory(); }
			size_t           Offset()const { return control.offset; }
			size_t           Size()const { return control.size; }
			Alloc(Memories& mem, uint32_t index, size_t offset, size_t size) :control{ &mem,index,offset,size } {}
			Alloc(const Alloc&) = delete;
			Alloc(Alloc&&) = default;
			~Alloc();
		private:
			struct Control
			{
				uint32_t index;
				Memories* src;
				size_t offset;
				size_t size;
				Control(Memories* mem, uint32_t idx, size_t o, size_t sz) : src{ mem }, index{ idx }, offset{ o }, size{ sz }{}
				Control(Control&& mv) :index{ mv.index }, src{ mv.src }, offset{ mv.offset }, size{ mv.size }{mv.src = nullptr; }
				vk::DeviceMemory Memory()const;
				Memories::Memory& IntMemory()const;
				~Control();
			};
			Control control;
		};
		using UniqueAlloc =std::unique_ptr<Alloc>;

		UniqueAlloc Allocate(vk::Buffer& buffer, vk::MemoryPropertyFlags prop);
		UniqueAlloc Allocate(vk::PhysicalDevice pd, vk::Device d, vk::Buffer& buffer, vk::MemoryPropertyFlags prop);
	private:
		vk::Device device;
		vk::PhysicalDevice pdevice;
		hash_table<decltype(vk::MemoryRequirements::memoryTypeBits), Memories> memories;
	};
	using UniqueAlloc =MemoryAllocator::UniqueAlloc;
}