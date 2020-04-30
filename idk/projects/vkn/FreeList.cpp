#include "pch.h"
#include "FreeList.h"
namespace idk
{
	FreeList::FreeList() = default;
	FreeList::FreeList(FreeList&& rhs):
		_head{ rhs._head },
		_last_prev{ rhs._last_prev },
		_last_stop{ rhs._last_stop },
		_allocator{ std::move(rhs._allocator) }
	{
		rhs._head = nullptr;
		rhs._last_prev= nullptr;
		rhs._last_stop = nullptr;
	}
	FreeList& FreeList::operator=(FreeList&& rhs)
	{
		std::swap(rhs._allocator, _allocator);
		std::swap(rhs._head, _head);
		std::swap(rhs._last_prev, _last_prev);
		std::swap(rhs._last_stop, _last_stop);

		return *this;
	}
	FreeList::~FreeList()
	{
		destroy(_head);
	}
	bool FreeList::CanAllocate(size_t size)const
	{
		_last_stop = nullptr;
		auto itr = _head;
		Record* prev = nullptr;
		while (itr)
		{
			if (itr->size >= size)
			{
				_last_stop = itr;
				break;
			}
			prev = itr;
			itr = itr->next;
		}

		return _last_stop != nullptr;
	}
	namespace vkn
	{
		size_t Aligned(size_t offset, size_t alignment);
	}
	size_t FreeList::Allocate(size_t size,size_t alignment)
	{
		auto search_to = [this,alignment](size_t size,Record* prev, Record* start,Record* end)
		{
			size_t result = static_cast<size_t>(-1);
			auto itr = start;
			while (itr!=end)
			{
				auto aligned_offset =vkn::Aligned(itr->offset, alignment);
				auto aligned_diff = aligned_offset - itr->offset;
				if (itr->size >= size+aligned_diff)
				{
					result = itr->offset;
					itr->offset += size+aligned_diff;
					itr->size -= size + aligned_diff;
					if (itr->size == 0)
					{
						if (prev)
						{
							remove_next(prev);
						}
						else
						{
							auto tmp = _head;
							_head = _head->next;
							tmp->set_next(nullptr);
							destroy(tmp);
						}
					}
					break;
				}
				prev = itr;
				itr = itr->next;
			}
			return result;
		};
		auto result = search_to(size, _last_prev, _last_stop, nullptr);
		_last_prev = _last_stop = nullptr;
		if (result != static_cast<size_t>(-1))
			return result;
		result = search_to(size, nullptr, _head, _last_stop);
		return result;
	}
		void FreeList::Free(size_t offset, size_t size)
		{
			static int counter = 0;
			++counter;
			if (size == 0)
				return;
			auto itr = _head;
			Record** insert_ptr = &_head;
			bool handled = false;
			while (itr)
			{
				if (itr->offset == offset) //Double deleting?
					DebugBreak();
				//new guy is infront and connects with current node's start exactly
				if (itr->offset == offset + size)
				{
					itr->offset = offset;
					itr->size += size;
					handled = true;
					break;
				}
				else if (itr->offset < offset) //new guy is after the current node's start
				{
					//Current node connects exactly with new guy's front
					if (itr->offset + itr->size == offset)
					{
						itr->size += size;
						auto next = itr->next;
						while (next && itr->offset + itr->size == next->offset)
						{
							itr->size += next->size;
							remove_next(itr);
							next = itr->next;
						}
						handled = true;
						break;
					}
					////nothing next or new guy is infront of next guy
					//else if (!itr->next || itr->next->offset > offset)
					//{
					//	insert(&(*insert_ptr)->next, construct(Record{ offset, size }));
					//	handled = true;
					//}
					//if(handled)
					//	break;
				}
				else //New guy is infront, but does not connect exactly with current
				{
					//None of the subsequent nodes can infront of new guy, insert now.
					insert(insert_ptr, construct(Record{ offset, size }));
					handled = true;
					break;
				}
				insert_ptr = &itr->next;
				itr = itr->next;
			}
			//auto pre_insert = *insert_ptr;
			if (!handled)
			{
				insert(insert_ptr, construct(Record{ offset, size }));
			}
			//auto tmp = _head;
			//while (tmp && tmp->next)
			//{
			//	if (tmp->offset > tmp->next->offset)
			//		DebugBreak();
			//	tmp = tmp->next;
			//}

		}
		void FreeList::clear()
		{
			destroy(_head);
			_head = nullptr;
			_last_stop = nullptr;
			_last_prev = nullptr;
		}
		std::optional<std::pair<size_t, size_t>> FreeList::PopBoundary(size_t boundary)
		{
			auto itr = _head;
			auto prev_ptr = _head;
			while (itr)
			{
				if (itr->offset == boundary || itr->offset + itr->size == boundary)
				{
					auto result = std::make_pair(itr->offset, itr->size);
					if (_head == itr)
					{
						_head = itr->next;
						itr->set_next( nullptr);
						destroy(itr);
					}
					else
					{
						remove_next(prev_ptr);
					}
					return result;
				}
				prev_ptr = itr;
				itr = itr->next;
			}
			return {};
		}
		FreeList::Record* FreeList::get_end(Record* r)
		{
			while (r && r->next)
			{
				r = r->next;
			}
			return r;
		}
		void FreeList::remove_next(Record* curr)
		{
			auto next = curr->next;
			curr->set_next(curr->next->next);
			next->set_next(nullptr);
			destroy(next);
		}
		void FreeList::insert(Record** insert_ptr, Record* rec)
		{
			auto& head = *insert_ptr;
			Record* tmp = head;
			head = rec;
			Record* end = get_end(rec);
			//assert(end->next==nullptr)
			end->set_next(tmp);
			//end->next = tmp;
		}
		FreeList::Record* FreeList::construct(Record&& r)
		{
			return new (_allocator.allocate()) Record{ std::move(r) };
		}
		void FreeList::destroy(Record* r)
		{
			if (r)
			{
				if (r->next)
				{
					destroy(r->next);
					r->next = nullptr;
				}
				r->~Record();
				_allocator.deallocate(r);
			}
		}
}
