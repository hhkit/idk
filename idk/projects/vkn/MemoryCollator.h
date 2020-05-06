#pragma once
#include <idk.h>
#include <ds/index_span.h>

#include <vkn/FreeList.h>
namespace idk::vkn
{
	size_t Aligned(size_t offset, size_t alignment);
	

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
			bool overlap_excluding_boundary(const MemoryRange& range)const noexcept;
			bool overlaps(const MemoryRange& range)const noexcept;
			//returns true if absorbed (overlapping)
			bool absorb(const MemoryRange& range)noexcept;
			bool can_split(size_t sz, size_t align)const noexcept;
			operator bool()const noexcept;
			std::optional<MemoryRange> split(size_t sz, size_t align)noexcept;
		};

		MemoryCollator(index_span allocated_span) :full_range{ allocated_span } { reset(); }
		bool skip_free = false;
		//unaligned, aligned offsets
		bool can_allocate(size_t size, size_t alignment) const;
		std::optional<std::pair<size_t, size_t>> allocate(size_t size, size_t alignment);

		void mark_freed(index_span to_free);
		void reset();
		index_span full_range{};
		//MemoryRange free_range{};
		FreeList free_list;
		using index_t = size_t;
		std::map<index_t, size_t> allocated;
		void validate();
		//std::set<MemoryRange> free_list;

	};
}