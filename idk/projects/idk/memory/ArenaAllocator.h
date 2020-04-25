#pragma once
namespace idk
{
	template<typename T, typename U>
	T* align(U* a) noexcept
	{
		auto ptr = reinterpret_cast<size_t>(a);
		constexpr size_t align_of = alignof(T);
		auto mod = (ptr % align_of);
		return reinterpret_cast<T*>(ptr + (mod ? align_of - mod : 0));
	}


namespace detail
{

	template<template<typename...>typename TM, typename T>
	struct IsAllocator : std::false_type {};

	template<template<typename...>typename TM, typename ...Args>
	struct IsAllocator<TM, TM<Args...>> :std::true_type {};

}

template<template<typename>typename TM, typename T>
struct IsAllocator : detail::IsAllocator<TM, std::decay_t<T>> {};

//MUST HAVE TRIVIAL DESTRUCTOR, we will not be destroying this. (Avoiding overhead of refcounting)
struct arena_block
{
	using ctrl_block_ptr_t = arena_block *;

	static ctrl_block_ptr_t init(unsigned char* ptr,size_t bytes)
	{
		auto aligned_ptr = align<arena_block>(ptr);
		auto start = aligned_ptr + 1;
		new (aligned_ptr) arena_block{ start, bytes - static_cast<size_t>(reinterpret_cast<unsigned char*>(start) - ptr) };
		return aligned_ptr;
	}

	unsigned char* arena{};
	unsigned char* next{};
	unsigned char* end{};
	template<typename Y>
	arena_block(Y* pool, size_t bytes) noexcept :
		arena{ reinterpret_cast<unsigned char*>(pool) },
		next{ arena }
		, end{ arena + bytes }
	{
	}
	template<typename T>
	T* try_allocate(size_t num_bytes)
	{
		auto result = align<T>(next);
		auto new_next = reinterpret_cast<unsigned char*>(result ) + num_bytes;

		if (new_next > end)
		{
			return nullptr;
		}
		next = new_next;
		return result;
	}

	template<typename T>
	bool try_release(T* ptr, size_t N) noexcept
	{
		auto cptr = reinterpret_cast<unsigned char*>(ptr);
		bool release = cptr+ N == next;
		if (release)
			next = cptr;
		

		return release;
	}
};

//std::ostream& operator<<(std::ostream& out, const arena_block& block)
//{
//	return out << "{" << (void*)block.arena << ": next:" << (void*)block.next << " , end:" << (void*)block.end << ", LEFT: " << (block.end - block.next) << "}";
//}
template<typename T,typename arena_block_t = arena_block>
struct ArenaAllocator
{
	using ptr_t = T *;
	using value_type = T;
	using size_type = size_t;
	using difference_type = ptrdiff_t;
	using reference = T &;
	using const_reference = const T &;
	using pointer = ptr_t;
	using const_pointer = const T*;

	template<typename A>
	using is_allocator_t = std::enable_if_t<IsAllocator<idk::ArenaAllocator, std::remove_const_t<std::remove_reference_t<A>>>::value>;
	template<typename A>
	using is_not_allocator_t = std::enable_if_t<!IsAllocator<idk::ArenaAllocator, std::remove_const_t<std::remove_reference_t<A>>>::value>;
	typename arena_block_t::ctrl_block_ptr_t arena = {};
	ArenaAllocator() { }
	~ArenaAllocator() {}
	template<typename Y>
	ArenaAllocator(Y* pool, size_t bytes) ;
	template<typename Y, size_t N>
	ArenaAllocator(Y(&pool)[N]) : ArenaAllocator{ pool, N * sizeof(Y) } {
	}
	template<typename Y>
	ArenaAllocator(span<Y> pool) : ArenaAllocator{ pool.data(), pool.size()* sizeof(Y) } {
	}
	template<typename Y, size_t N>
	ArenaAllocator(std::array<Y,N>& pool) : ArenaAllocator{ std::data(pool), N * sizeof(Y) } {
	}
	template<typename A, typename = is_allocator_t<A>>
	ArenaAllocator(A && a) : arena{ a.arena }
	{
	}
	template<typename A, typename = is_allocator_t<A>>
	ArenaAllocator & operator=(A&& a) noexcept
	{
		arena = a.arena;
		return *this;
	}
	struct alignas(T) empty
	{
		char a[sizeof(T)];
	};
	ptr_t allocate_fallback(size_t n);
	void deallocate_fallback(ptr_t ptr);
	ptr_t allocate(size_t n);
	void deallocate(ptr_t ptr, size_t N)noexcept;
	template<typename A, typename = is_allocator_t<A>>
	bool operator==(const A & rhs)const noexcept
	{
		return arena == rhs.arena;
	}
	bool operator!=(const ArenaAllocator& rhs)const noexcept
	{
		return *this != rhs;
	}
	template<typename U>
	struct rebind
	{
		using other = ArenaAllocator<U>;
	};
};
}