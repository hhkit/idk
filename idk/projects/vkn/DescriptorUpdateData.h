#pragma once
#include <idk.h>
#include <vulkan/vulkan.hpp>
#include <forward_list>
#include <idk/memory/ArenaAllocator.inl>
namespace idk
{
	template<typename T, size_t ChunkSize = 16>
	struct PoolAllocator
	{
		struct Chunk
		{
			static constexpr size_t num_bytes = sizeof(T) * ChunkSize;
			alignas(T) std::array<std::byte, num_bytes> chunk;
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



		T* allocate()
		{
			if (!_free_head)
			{
				NewChunk();
			}
			return pop_free();
		}
		void deallocate(T* ptr)
		{
			new (ptr) T* { reinterpret_cast<T*>(_free_head) };
			_free_head = ptr;
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
		std::unique_ptr<Chunk> _head;
		T* _free_head = nullptr;
	};

	struct FreeList
	{
		struct Record
		{
			size_t offset;
			size_t size;
			Record* next = nullptr;
			Record(size_t o = 0, size_t sz = 0, Record * nxt = nullptr) :offset{ o }, size{ sz }, next{ nxt }{}
			Record(const Record&) = delete;
			Record(Record&& other) : offset{ other.offset }, size{ other.size }, next{ other.next }
			{
				other.next = nullptr;
				other.offset = {};
				other.size = {};
			}
			Record& operator=(const Record&) = delete;
			Record& operator=(Record&& other)
			{
				std::swap(other.next, next);
				std::swap(other.offset, offset);
				std::swap(other.size, size);
				return *this;
			}
			~Record()
			{
				delete next;
				next = nullptr;
			}
		};
		size_t Allocate(size_t size);
		void Free(size_t offset, size_t size);

		std::optional<std::pair<size_t, size_t>> PopBoundary(size_t boundary);

	private:
		static Record* get_end(Record* r);
		void remove_next(Record* curr);
		static void insert(Record** insert_ptr, Record* rec);
		Record* construct(Record&& r);
		void destroy(Record* r);
		Record* _head = nullptr;
		PoolAllocator<Record> _allocator{};
	};
	struct arena_block_free;
	void free_block(arena_block_free* ptr);
	struct arena_block_free
	{
		using ctrl_block_ptr_t = std::shared_ptr<arena_block_free>;
		template<typename T>
		static std::pair<T*, T*> emplace_align(void* ptr)
		{
			auto aligned_ptr = align<T>(ptr);
			auto start = aligned_ptr + 1;
			return { aligned_ptr,start };
		}
		static ctrl_block_ptr_t init(unsigned char* ptr, size_t bytes)
		{
			//auto [sptr, next] = emplace_align<ctrl_block_ptr_t>(ptr);
			auto [aligned, start] = emplace_align <arena_block_free>(ptr);
			new (aligned) arena_block_free{ start, bytes - static_cast<size_t>(reinterpret_cast<unsigned char*>(start) - ptr) };
			return ctrl_block_ptr_t{ aligned ,[](arena_block_free* ptr) {ptr->~arena_block_free();} };
			;
			//{
			//
			//	aligned_ptr = align<ctrl_block_ptr_t>(start);
			//	auto start = aligned_ptr + 1;
			//	new (aligned_ptr) arena_block_free{ start, bytes - static_cast<size_t>(reinterpret_cast<unsigned char*>(start) - ptr) };
			//}
			//
			//return aligned_ptr;
		}
		unsigned char* arena{};
		unsigned char* next{};
		unsigned char* rnext{};
		unsigned char* end{};
		bool use_reverse = false;
		template<typename Y>
		arena_block_free(Y* pool, size_t bytes) noexcept :
			arena{ reinterpret_cast<unsigned char*>(pool) },
			next{ arena },
			rnext{ arena+bytes },
			end{ arena + bytes }
		{
			
		}
		template<typename T>
		T* plain_alloc(size_t num_bytes)
		{
			if (use_reverse)
			{
				//assert(rnext);
				auto start = rnext - (num_bytes+alignof(T));
				auto result = align<T>(start);
				auto new_next = reinterpret_cast<unsigned char*>(result);

				if (new_next < next)
				{
					return nullptr;
				}
				auto offset = start - arena;
				auto align_padding = reinterpret_cast<unsigned char*>(result) - start;
				_free_list.Free(offset+num_bytes+ align_padding, alignof(T) - align_padding);
				rnext = new_next;
				use_reverse = !use_reverse;
				return result;
			}
			else
			{

			auto result = align<T>(next);
			auto new_next = reinterpret_cast<unsigned char*>(result) + num_bytes;

			if (new_next > rnext)
			{
				return nullptr;
			}
			auto offset = next - arena;
			auto align_padding = reinterpret_cast<unsigned char*>(result) - next;
			if (align_padding)
				_free_list.Free(offset, align_padding);
			next = new_next;
			use_reverse = !use_reverse;
			return result;

			}
		}


		template<typename T>
		T* try_allocate(size_t num_bytes)
		{
			auto offset = _free_list.Allocate(num_bytes + alignof(T));
			if (offset > static_cast<size_t>(end - arena))
			{
				return plain_alloc<T>(num_bytes);
			}
			auto unaligned = arena + offset;
			auto res = align<T>(unaligned);
			auto align_padding = reinterpret_cast<unsigned char*>(res) - unaligned;
			_free_list.Free(offset + align_padding + num_bytes, alignof(T) - align_padding);
			_free_list.Free(offset, align_padding);
			return res;
		}
		unsigned char* pop_boundary(unsigned char* boundary, ptrdiff_t sign)
		{
			
			auto bound = _free_list.PopBoundary(boundary- arena);
			if (bound)
			{
				boundary -= sign*bound->second;
			}
			return boundary;
		}
		template<typename T>
		bool try_release(T* ptr, size_t N) noexcept
		{
			auto cptr = reinterpret_cast<unsigned char*>(ptr);
			bool release = cptr + N == next;
			if (release)
				next = cptr;
			else if (arena <= cptr && cptr < end)
			{
				_free_list.Free(cptr - arena, N);
				next = pop_boundary(next, 1);
				rnext = pop_boundary(rnext, -1);
				release = true;
			}

			return release;
		}
		FreeList _free_list;
	};
}

namespace idk::vkn
{
	



	namespace detail
	{
		template<typename T, size_t N>
		static constexpr size_t num_bytes = std::max(sizeof(T) * N, sizeof(size_t) * N);
	}
	namespace detail
	{
		template<size_t NumBytes>
		struct FixedInternal
		{
			std::array<char, NumBytes> buffer;
		};
	}


	struct DescriptorUpdateData
	{
		static void* derp(void* ptr);
		template<typename T>
		static T* test_fa(T* ptr)
		{
			return reinterpret_cast<T*>(derp(ptr));
		}

		static void JustBreak();

		template<typename T, size_t NumBytes=1<<16,typename base_t = ArenaAllocator<T,arena_block_free>>
		struct FixedArenaAllocator : base_t
		{
			template<typename T>
			static ArenaAllocator<T>& GetAllocatorBase(ArenaAllocator<T>& base) noexcept
			{
				return base;
			}

			using Internal = detail::FixedInternal<NumBytes>;
			template<typename U>
			struct rebind
			{
				using other = FixedArenaAllocator<U,NumBytes>;
			};
			std::shared_ptr<Internal> _internal;
			template<typename U, typename Base>
			bool operator==(const FixedArenaAllocator<U, NumBytes, Base>& rhs) noexcept
			{
				return _internal == rhs._internal;
			}
			//alignas(T) std::array<char, sizeof(T) * 2048> buffer;
			FixedArenaAllocator() 
			{
				_internal = std::make_shared<Internal>();
				base_t* ptr = this;
				new (ptr) base_t{ _internal->buffer };
			}
			FixedArenaAllocator(const FixedArenaAllocator& rhs) = default;
			template<
				typename U,
				typename = 
					std::enable_if_t<!std::is_same_v<FixedArenaAllocator<T,NumBytes>, FixedArenaAllocator<U, NumBytes> >>
			>
			FixedArenaAllocator(const FixedArenaAllocator<U, NumBytes>& rhs) : base_t{ GetAllocatorBase(rhs) }, _internal{ rhs._internal } {  }
			
			FixedArenaAllocator& operator=(const FixedArenaAllocator&) = delete;
			template<
				typename U,
				typename = 
					std::enable_if_t<!std::is_same_v<FixedArenaAllocator<T, NumBytes>, FixedArenaAllocator<U, NumBytes> >>
			>
			FixedArenaAllocator& operator=(const FixedArenaAllocator<U, NumBytes>& ) =delete;
			~FixedArenaAllocator() = default;
		};
		template<typename T>
		using vector_a = std::vector<T, FixedArenaAllocator<T>>;
		
		vector_a<vk::WriteDescriptorSet> descriptorWrite;
		std::forward_list<vector_a<vk::DescriptorImageInfo>> image_infos;
		std::forward_list<vk::DescriptorBufferInfo> buffer_infos;

		vector_a<vk::WriteDescriptorSet> scratch_descriptorWrite;
		vector_a<vector_a<vk::DescriptorImageInfo>> scratch_image_info;
		std::forward_list<vk::DescriptorBufferInfo> scratch_buffer_infos;
		void AbsorbFromScratch();
		void SendUpdates();
		void Reset();
	};
}