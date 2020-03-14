#pragma once
#include <idk.h>
#include <ds/index_span.h>
namespace idk::vkn
{
	class MemoryCollator
	{
	public:
		using offset_t = size_t;

		struct MemoryRange :index_span
		{
			using index_span::index_span;
			MemoryRange() = default;
			MemoryRange(index_span sp) :index_span{ sp } {}
			bool operator<(const MemoryRange& range)const noexcept {
				return _end < range._begin;
			}
			bool overlaps(const MemoryRange& range)const noexcept;
			//returns true if absorbed (overlapping)
			bool absorb(const MemoryRange& range)noexcept;
			bool can_split(size_t sz, size_t align)const noexcept;
			operator bool()const noexcept;
			std::optional<MemoryRange> split(size_t sz, size_t align)noexcept;
		};

		MemoryCollator(index_span allocated_span) :full_range{ allocated_span }, free_range{ allocated_span }{}
		//unaligned, aligned offsets
		bool can_allocate(size_t size, size_t alignment) const;
		std::optional<std::pair<size_t,size_t>> allocate(size_t size,size_t alignment);

		void mark_freed(index_span to_free);
		void reset();
		index_span full_range{};
		MemoryRange free_range{};
		std::set<MemoryRange> free_list;

	};
	size_t Aligned(size_t offset, size_t alignment);
	

}