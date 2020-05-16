#pragma once
#include <idk.h>
#include <vkn/PoolAllocator.h>
namespace idk
{
	struct FreeList
	{
		FreeList();
		FreeList(const FreeList&)=delete;
		FreeList(FreeList&&);
		FreeList& operator=(FreeList&&);
		~FreeList();
		struct Record
		{
			size_t offset;
			size_t size;
			Record* next = nullptr;
			size_t num_next = 0;
			Record(size_t o = 0, size_t sz = 0, Record * nxt = nullptr) :offset{ o }, size{ sz }, next{ nxt }, num_next{ next ? next->num_next + 1 : 0 }{}
			Record(const Record&) = delete;
			Record(Record&& other) : offset{ other.offset }, size{ other.size }, next{ other.next },num_next{other.num_next}
			{
				other.next = nullptr;
				other.offset = {};
				other.size = {};
				other.num_next = {};
			}
			Record& operator=(const Record&) = delete;
			Record& operator=(Record&& other)
			{
				std::swap(other.next, next);
				std::swap(other.offset, offset);
				std::swap(other.size, size);
				std::swap(other.num_next, num_next);
				return *this;
			}
			void set_next(Record* new_next)
			{
				//if(next != nullptr) throw std::runtime_error("attempting to set next without clearing next.");
				next = new_next;
				num_next = next ? next->num_next + 1 : 0;
			}
			~Record()
			{
				assert(next == nullptr);
				next = nullptr;
			}
		};
		bool CanAllocate(size_t size)const;
		size_t Allocate(size_t size, size_t alignment=1);
		void Free(size_t offset, size_t size);

		void clear();

		std::optional<std::pair<size_t, size_t>> PopBoundary(size_t boundary);

		//For debugging
		struct proxy
		{
			proxy(size_t sz):_size{sz}{}
			size_t size()const noexcept { return _size; }
		private:
			size_t _size=0;
		};

		struct rec_itr
		{
			rec_itr(const Record* record) :ptr{ record }{}
			proxy operator*()const { return ptr->size; }
			rec_itr& operator++() { ptr = ptr->next; return *this; }
			rec_itr operator++(int) { 
				rec_itr result{ ptr };
				ptr = ptr->next; 
				return result; 
			}
			bool operator==(const rec_itr& rhs)const noexcept
			{
				return rhs.ptr == ptr;
			}
			bool operator!=(const rec_itr& rhs)const noexcept
			{
				return !(*this == rhs);
			}
		private:
			const Record* ptr;
		};

		rec_itr begin()const
		{
			return _head;
		}
		rec_itr end()const
		{
			return nullptr;
		}
		size_t size()const
		{
			return (_head) ? _head->num_next + 1 : 0;
		}


	private:
		static Record* get_end(Record* r);
		void remove_next(Record* curr);
		static void insert(Record** insert_ptr, Record* rec);
		Record* construct(Record&& r);
		void destroy(Record* r);
		Record* _head = nullptr;

		mutable Record* _last_stop = nullptr, *_last_prev=nullptr;
		PoolAllocator<Record> _allocator{};
	};
}