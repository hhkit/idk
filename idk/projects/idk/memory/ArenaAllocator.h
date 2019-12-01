#pragma once
namespace idk
{

namespace detail
{

	template<template<typename>typename TM, typename T>
	struct IsAllocator : std::false_type {};

	template<template<typename>typename TM, typename T>
	struct IsAllocator<TM, TM<T>> :std::true_type {};

}

template<template<typename>typename TM, typename T>
struct IsAllocator : detail::IsAllocator<TM, std::decay_t<T>> {};

//MUST HAVE TRIVIAL DESTRUCTOR, we will not be destroying this. (Avoiding overhead of refcounting)
struct arena_block
{
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
	bool try_release(T* ptr, size_t N) noexcept
	{
		bool release = reinterpret_cast<unsigned char*>(ptr + N) == next;
		if (release)
			next = reinterpret_cast<unsigned char*>(ptr);
		return release;
	}
};
template<typename T, typename U>
T* align(U* a) noexcept
{
	auto ptr = reinterpret_cast<size_t>(a);
	constexpr size_t align_of = alignof(T);
	auto mod = (ptr % align_of);
	return reinterpret_cast<T*>(ptr + (mod ? align_of - mod : 0));
}


//std::ostream& operator<<(std::ostream& out, const arena_block& block)
//{
//	return out << "{" << (void*)block.arena << ": next:" << (void*)block.next << " , end:" << (void*)block.end << ", LEFT: " << (block.end - block.next) << "}";
//}

template<typename T>
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
	arena_block* arena = nullptr;
	ArenaAllocator() { }
	~ArenaAllocator() {}
	template<typename Y>
	ArenaAllocator(Y* pool, size_t bytes) noexcept;
	template<typename Y, size_t N>
	ArenaAllocator(Y(&pool)[N]) noexcept : ArenaAllocator{ pool, N * sizeof(Y) } {
	}

	template<typename A, typename = is_allocator_t<A>>
	ArenaAllocator(A && a) : arena{ a.arena }
	{
	}
	template<typename A, typename = is_allocator_t<A>>
	ArenaAllocator & operator=(A&& a) noexcept
	{
		return arena = a.arena;
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