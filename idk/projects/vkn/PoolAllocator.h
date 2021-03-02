#pragma once
#include <memory>
#include <mutex>
namespace idk
{
	template<size_t obj_size, size_t align, size_t ChunkSize>
	struct size_pool
	{
		static_assert(obj_size >= sizeof(void*));
		using T = std::array<std::byte, obj_size>;
		struct Chunk
		{
			static constexpr size_t num_bytes = sizeof(T) * ChunkSize;
			alignas(align) std::array<std::byte, num_bytes> chunk;
			std::unique_ptr<Chunk> next = {};
			Chunk(std::unique_ptr<Chunk> tail, T*& free_head) : next{ std::move(tail) }
			{
				constexpr auto incr = sizeof(T);
				for (size_t i = 0; i < num_bytes; i += incr)
				{
					auto ptr = std::data(chunk) + i;
					auto ptr_next = ptr + incr;
					ptr_next = (ptr_next < std::data(chunk) + num_bytes) ? ptr_next : reinterpret_cast<byte*>(free_head);
					new (ptr) T* { reinterpret_cast<T*>(ptr_next) };
				}
				free_head = reinterpret_cast<T*>(std::data(chunk));
			}
		};
		struct Internal
		{
			std::unique_ptr<Chunk> _head;
			T* _free_head = nullptr;
			void* allocate()
			{
				std::lock_guard guard{ lock };
				if (!_free_head)
				{
					NewChunk();
				}
				return pop_free();
			}
			void deallocate(void* ptr)
			{
				std::lock_guard guard{ lock };
				new (ptr) T* { reinterpret_cast<T*>(_free_head) };
				_free_head = reinterpret_cast<T*>(ptr);
			}
		private:
			T* pop_free() //check if free is empty before popping
			{
				auto result = _free_head;
				//if(_free_head)
				_free_head = *reinterpret_cast<T * *>(_free_head);
				return result;
			}
			void NewChunk()
			{
				_head = std::make_unique<Chunk>(std::move(_head), _free_head);
			}
			std::mutex lock;
		};
		//maybe change to thread local to be thread safe?

		static Internal& GetInst()
		{
			static Internal i;
			return i;
		}

		static void* allocate()
		{
			return GetInst().allocate();
		}
		static void deallocate(void* ptr)
		{
			GetInst().deallocate(ptr);
		}
	private:
	};
	template<typename T, size_t ChunkSize = 16>
	struct PoolAllocator
	{
		using value_type = typename T;
		using size_type = typename size_t;
		using difference_type = typename ptrdiff_t;
		using reference = typename T &;
		using const_reference = typename const T &;
		using pointer = typename T*;
		using const_pointer = typename const T*;

		template<typename U>
		struct rebind
		{
			using other = PoolAllocator<U, ChunkSize>;
		};


		using pool_t = size_pool<sizeof(T), alignof(T), ChunkSize>;

		template<typename U>
		bool operator==(const PoolAllocator<U, ChunkSize>& rhs)const noexcept
		{
			return &pool_t::GetInst() == (typename PoolAllocator<U, ChunkSize>::pool_t)::GetInst();
		}

		T* allocate(size_t n = 1)
		{
			if (n > 1)
				throw std::runtime_error("Attempting to allocate more than one object from a pool allocator at a time.");
			return reinterpret_cast<T*>(pool_t::allocate());
			//if (!_free_head)
			//{
			//	NewChunk();
			//}
			//return pop_free();
		}
		void deallocate(T* ptr, size_t = 1)
		{
			pool_t::deallocate(ptr);
			//new (ptr) T* { reinterpret_cast<T*>(_free_head) };
			//_free_head = ptr;
		}
	private:
	};

}