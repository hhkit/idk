#pragma once
#include <idk.h>
#include <vulkan/vulkan.hpp>
size_t Track(size_t s);
namespace idk::vkn::hlp
{
	namespace detail
	{

	struct Memories
	{
		struct Memory
		{
			vk::UniqueDeviceMemory memory;
			size_t sz{};
			size_t curr_offset{};

			Memory(vk::UniqueDeviceMemory&& mem, size_t size) :memory{ std::move(mem) }, sz{ size }{}

			void Free(size_t offset, size_t size);
			//Returns offset if it is allocated
			std::optional<size_t> Allocate(size_t size,size_t alignment);
		};
		vk::Device device;
		uint32_t type;
		vector<Memory> memories;
		size_t chunk_size{};
		Memories(
			vk::Device d,
			uint32_t mem_type,
			size_t chunkSize = 1 << 24 //16MB
		);
		Memory& Add();
		std::pair<uint32_t, size_t> Allocate(size_t size, size_t alignment)
		{
			std::optional<size_t> alloc_offset;
			uint32_t index{};
			for (auto& mem : memories)
			{
				alloc_offset = mem.Allocate(size,alignment);
				if (alloc_offset)
				{
					break;
				}
				++index;
			}
			if (!alloc_offset)
			{
				auto& mem = Add();
				alloc_offset = mem.Allocate(size,alignment);
			}
			return std::make_pair(index, *alloc_offset);
		}
	};

	}
	class MemoryAllocator
	{
	public:
		using Memories = detail::Memories;
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

	MemoryAllocator(vk::Device d, vk::PhysicalDevice pd):device{d},pdevice{pd}{}

		UniqueAlloc Allocate(vk::Device d, uint32_t mem_type, vk::MemoryRequirements mem_req);
		UniqueAlloc Allocate(vk::Image image, vk::MemoryPropertyFlags prop);
		UniqueAlloc Allocate(vk::Buffer& buffer, vk::MemoryPropertyFlags prop);
		UniqueAlloc Allocate(vk::PhysicalDevice pd, vk::Device d, vk::Buffer& buffer, vk::MemoryPropertyFlags prop);
	private:
		vk::Device device;
		vk::PhysicalDevice pdevice;
		hash_table<decltype(vk::MemoryRequirements::memoryTypeBits), Memories> memories;
	};
	using UniqueAlloc =MemoryAllocator::UniqueAlloc;
}