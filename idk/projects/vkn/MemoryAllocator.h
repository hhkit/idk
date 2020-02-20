#pragma once
#include <idk.h>
#include <vulkan/vulkan.hpp>
#include <vkn/SimpleLock.h>
size_t Track(size_t s);
namespace idk::vkn::hlp
{
	namespace detail
	{

	struct Memories
	{
		struct Memory
		{
			Memory(vk::UniqueDeviceMemory&& mem, size_t size) :memory{ std::move(mem) }, sz{ size }{}

			void Free(size_t offset, size_t size);
			//Returns offset if it is allocated
			std::optional<std::pair<size_t, size_t>> Allocate(size_t size,size_t alignment);

			vk::UniqueDeviceMemory memory;
			size_t sz{};
			size_t curr_offset{};

			struct MemoryRange
			{
				bool operator<(const MemoryRange& range)const noexcept {
					return end < range.start;
				}
				bool overlaps(const MemoryRange& range)const noexcept;
				//returns true if absorbed (overlapping)
				bool absorb(const MemoryRange& range)noexcept;
				bool can_split(size_t sz,size_t align)const noexcept;
				operator bool()const noexcept;
				std::optional<MemoryRange> split(size_t sz, size_t align)noexcept;
				size_t start={}, end={};
			};

			std::set<MemoryRange> free_list;

		};
		vk::Device device;
		uint32_t type;
		vector<Memory> memories;
		size_t chunk_size{};
		static constexpr size_t default_chunk_size = 8847360;/* Memory gets fragmented above this. */// 1 << 24; //64MB (2048*2048*4) 
		Memories(
			vk::Device d,
			uint32_t mem_type,
			size_t chunkSize = default_chunk_size 
		);
		Memory& Add(size_t min_size);
		std::pair<uint32_t, std::pair<size_t, size_t>> Allocate(size_t size, size_t alignment);
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
			size_t           BlockSize()const { return control.IntMemory().sz; }
			Alloc(Memories& mem, uint32_t index,size_t u_offset, size_t offset, size_t size) :control{ &mem,index,u_offset,offset,size } {}
			Alloc(const Alloc&) = delete;
			Alloc(Alloc&&) = default;
			~Alloc();
		private:
			struct Control
			{
				uint32_t index;
				Memories* src;
				size_t offset;
				size_t unaligned_offset;
				size_t size;
				Control(Memories* mem, uint32_t idx, size_t u_o, size_t o, size_t sz) : src{ mem }, index{ idx }, unaligned_offset{u_o},offset{ o }, size{ sz }{}
				Control(Control&& mv) noexcept :index{ mv.index }, src{ mv.src }, unaligned_offset{ unaligned_offset }, offset{ mv.offset }, size{ mv.size }{mv.src = nullptr; }
				vk::DeviceMemory Memory()const;
				Memories::Memory& IntMemory()const;
				~Control();
			};
			Control control;
		};
		using UniqueAlloc =std::unique_ptr<Alloc>;

		MemoryAllocator();
		MemoryAllocator(vk::Device d, vk::PhysicalDevice pd);
		MemoryAllocator(const MemoryAllocator&) = delete;
		MemoryAllocator(MemoryAllocator&&) = default;
		MemoryAllocator& operator=(const MemoryAllocator&) = delete;
		MemoryAllocator& operator=(MemoryAllocator&&) = default;

		UniqueAlloc Allocate(vk::Device d, uint32_t mem_type, vk::MemoryRequirements mem_req);
		UniqueAlloc Allocate(vk::Image image, vk::MemoryPropertyFlags prop);
		UniqueAlloc Allocate(vk::Buffer& buffer, vk::MemoryPropertyFlags prop);
		UniqueAlloc Allocate(vk::PhysicalDevice pd, vk::Device d, vk::Buffer& buffer, vk::MemoryPropertyFlags prop);
		~MemoryAllocator();
		friend string DumpAllocators();
	
		friend std::pair<size_t, size_t> DumpAllocator(std::ostream& out, const MemoryAllocator& alloc);
	private:
		vk::Device device;
		vk::PhysicalDevice pdevice;
		hash_table<decltype(vk::MemoryRequirements::memoryTypeBits), Memories> memories;
		SimpleLock lock;
	};
	using UniqueAlloc =MemoryAllocator::UniqueAlloc;
}