#include "pch.h"
#include "MemoryCollator.h"
namespace idk::vkn
{
//#pragma optimize("",off)
	bool MemoryCollator::can_allocate(size_t size, size_t alignment) const
	{
		bool result = false;
		for (auto& range : free_list)
		{
			if (range.can_split(size, alignment))
			{
				result = true;
				break;
			}
		}

		result |= (!result && free_range.can_split(size, alignment));
		return result;
	
	}
	std::optional<std::pair<size_t, size_t>> MemoryCollator::allocate(size_t size,size_t alignment)
	{
		std::optional<MemoryRange> result_range;
		for (auto& range : free_list)
		{
			if (range.can_split(size, alignment))
			{
				auto new_range = range;
				result_range = new_range.split(size, alignment);
				free_list.erase(range);
				if (new_range)
					free_list.emplace(new_range);
				break;
			}
		}

		if (!result_range && free_range.can_split(size, alignment))
		{
			result_range = *free_range.split(size, alignment);
			//
		}

		return result_range ? std::optional<std::pair<size_t, size_t>>{std::pair<size_t, size_t>{ result_range->_begin, Aligned(result_range->_begin, alignment) }} : std::nullopt;
	}

	void MemoryCollator::mark_freed(index_span to_free)
	{
		MemoryRange range = to_free;
		//TODO use some kind of find closest (maybe upper bound)?
		for (auto& freed : free_list)
		{
			if (freed.overlaps(range))
			{
				auto new_range = freed;
				free_list.erase(freed);
				new_range.absorb(range);
				auto itr = free_list.lower_bound(new_range);
				if (itr != free_list.end() && itr->overlaps(new_range))
				{
					new_range.absorb(*itr);
					free_list.erase(itr);
				}
				range = new_range;
				break;
			}
		}
		if (range._end == free_range._begin)
			free_range._begin = range._begin;
		else
			free_list.emplace(range);
	}

	void MemoryCollator::reset()
	{
		free_list.clear();
		free_range = full_range;
	}

	size_t Aligned(size_t offset, size_t alignment)
	{
		size_t mod = (offset) % alignment;
		return offset + ((mod) ? (alignment - mod) : 0);
	}

	//returns true if absorbed (overlapping)

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