#include "pch.h"
#include "DescriptorUpdateData.h"
#include <vkn/VulkanView.h>
namespace idk::vkn
{
	void* DescriptorUpdateData::derp(void* ptr)
	{
		//while (true);
		return ptr;
	}
	void DescriptorUpdateData::JustBreak()
	{
	//	while (true);
	}
	void DescriptorUpdateData::AbsorbFromScratch()
{
	auto& dsw = scratch_descriptorWrite;
	descriptorWrite.insert(descriptorWrite.end(), dsw.begin(), dsw.end());
	for (auto& img_info : scratch_image_info)
	{
		image_infos.emplace_front(std::move(img_info));
	}

	//buffer_infos.reserve(buffer_infos.size() + scratch_buffer_info.size());
	buffer_infos.splice_after(buffer_infos.before_begin(), std::move(scratch_buffer_infos));
	dsw.clear();
	scratch_image_info.clear();
	scratch_buffer_infos.clear();
}
VulkanView& View();
void DescriptorUpdateData::SendUpdates()
{
	auto& device = *View().Device();
	device.updateDescriptorSets(descriptorWrite, nullptr, vk::DispatchLoaderDefault{});
}

void DescriptorUpdateData::Reset()
{
	descriptorWrite.clear();
	image_infos.clear();
	buffer_infos.clear();
}

static bool some_value = false;
static bool some_toggle = false;
bool some_func()
{
	bool value = some_value;
	some_value = some_toggle;
	return value;
}

}
namespace idk
{
	size_t FreeList::Allocate(size_t size)
	{
		auto itr = _head;
		Record* prev = nullptr;
		size_t result = static_cast<size_t>(-1);
		while (itr)
		{
			if (itr->size >= size)
			{
				result = itr->offset;
				itr->offset += size;
				itr->size -= size;
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
						tmp->next = nullptr;
						tmp->~Record();
						_allocator.deallocate(tmp);
					}
				}
				break;
			}
			prev = itr;
			itr = itr->next;
		}
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
			//new guy is infront and connects with current node's start exactly
			if (itr->offset == offset + size)
			{
				itr->offset = offset;
				itr->size += size;
				handled = true;
				break;
			}else if (itr->offset < offset) //new guy is after the current node's start
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
		auto pre_insert = *insert_ptr;
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
					itr->next = nullptr;
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
		curr->next = curr->next->next;
		next->next = nullptr;
		next->~Record();
		_allocator.deallocate(next);
	}
	void FreeList::insert(Record** insert_ptr, Record* rec)
	{
		auto& head = *insert_ptr;
		Record* tmp = head;
		head = rec;
		Record* end = get_end(rec);
		//assert(end->next==nullptr)
		end->next = tmp;
	}
	FreeList::Record* FreeList::construct(Record&& r)
	{
		return new (_allocator.allocate()) Record{ std::move(r) };
	}
	void FreeList::destroy(Record* r)
	{
		r->~Record();
		_allocator.deallocate(r);
	}
	void free_block(arena_block_free* ptr) 
	{
		while (true);
		ptr->~arena_block_free(); 
	}
}