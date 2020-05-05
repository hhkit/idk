#include "pch.h"
#include "MemoryCollator.h"
namespace idk::vkn
{

	size_t Aligned(size_t offset, size_t alignment)
	{
		size_t mod = (offset) % alignment;
		return offset + ((mod) ? (alignment - mod) : 0);
	}

	size_t compute_aligned_alloc(size_t size, size_t alignment)
	{
		if (size == alignment && size == 0)
			throw std::runtime_error("Invalid allocation size!");
		return size + alignment ;
	}
	
	bool MemoryCollator::can_allocate(size_t size, size_t alignment) const
	{
		bool result = free_list.CanAllocate(compute_aligned_alloc(size, alignment));
		return result;

	}
	std::optional<std::pair<size_t, size_t>> MemoryCollator::allocate(size_t size, size_t alignment)
	{
		static size_t allocation_count = 0;
		std::optional<MemoryRange> result_range;

		{
			const auto offset = free_list.Allocate(size,alignment);
			allocation_count++;
			if (full_range._begin != 0)
				throw;
			if (offset < full_range.size())
			{
				if (offset == (size_t)-1)
					throw;
				const auto aligned = Aligned(offset, alignment);
				result_range = index_span{ offset,aligned + size };
			}
		}

		std::optional<std::pair<size_t, size_t>> result{};
		if (result_range)
		{
			result = { result_range->_begin, Aligned(result_range->_begin, alignment) };
			//if(!allocated.emplace(result_range->_begin,result_range->size()).second)
			//	DebugBreak();
			//validate();
		}
		return result;
	}

	void MemoryCollator::mark_freed(index_span to_free)
	{
		//bool freed = false;
		//auto itr = allocated.find(to_free._begin);
		//if (itr == allocated.end())
		//	DebugBreak();
		//if(itr->second < to_free.size())
		//	DebugBreak();
		//auto sz = itr->second;
		//allocated.erase(itr);
		//if (itr->second > to_free.size())
		//{
		//	allocated.emplace(to_free._end, sz - to_free.size());
		//}
		//validate();
		free_list.Free(to_free._begin, to_free.size());
	}

	void MemoryCollator::reset()
	{
		allocated.clear();
		free_list.clear();
		free_list.Free(full_range._begin, full_range.size());
	}
	static constexpr bool should_validate = false;
	void MemoryCollator::validate()
	{
		if (!should_validate)
			return;
		for (auto itr=allocated.begin(),end=allocated.end();itr!=end;++itr)
		{
			auto& [start, size] = *itr;
			for (auto itr2 = itr; ++itr2 != end;)
			{
				auto& [start2, size2] = *itr2;
				if (start + size <= start2)
					break;
				DebugBreak();
			}
		}
	}


	//returns true if absorbed (overlapping)

	bool MemoryCollator::MemoryRange::overlap_excluding_boundary(const MemoryRange& range) const noexcept
	{
		return !(_begin >= range._end || _end <= range._begin);
	}

	bool MemoryCollator::MemoryRange::overlaps(const MemoryRange& range) const noexcept
	{
		return !(_begin > range._end || _end < range._begin);
	}

	bool MemoryCollator::MemoryRange::absorb(const MemoryRange& range) noexcept
	{
		bool should_absorb = overlaps(range);
		_begin = (should_absorb) ? std::min(range._begin, _begin) : _begin;
		_end = (should_absorb) ? std::max(range._end, _end) : _end;
		return should_absorb;
	}
	bool MemoryCollator::MemoryRange::can_split(size_t sz, size_t align) const noexcept
	{
		auto aligned__begin = Aligned(_begin, align);
		auto new__begin = aligned__begin + sz;
		return (_end >= new__begin);
	}
	MemoryCollator::MemoryRange::operator bool()const noexcept
	{
		return _begin > _end;
	}
	std::optional<MemoryCollator::MemoryRange> MemoryCollator::MemoryRange::split(size_t sz, size_t align) noexcept
	{
		std::optional<MemoryCollator::MemoryRange> result{};
		auto aligned__begin = Aligned(_begin, align);
		auto new__begin = aligned__begin + sz;
		if (_end >= new__begin)
			result = MemoryRange{ _begin, new__begin - _begin };
		_begin = aligned__begin + sz;
		return result;
	}
}