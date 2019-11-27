#include "stdafx.h"
#include "ShaderIncluder.h"
#include <core/Core.h>
#include <gfx/ShaderSnippet.h>
namespace idk
{

	struct IncludeEntry
	{
		size_t start, len;
		string_view include_name;
	};
	IncludeEntry FindNextIncludes(string_view& str_view, size_t& offset) noexcept;

	string_view GetIncluded(string_view mounted_dir);
	namespace detail
	{

		template<template<typename>typename TM, typename T>
		struct IsAllocator : std::false_type{};

		template<template<typename>typename TM, typename T>
		struct IsAllocator<TM, TM<T>> :std::true_type{};

	}

	template<template<typename>typename TM, typename T>
	struct IsAllocator : detail::IsAllocator<TM, std::decay_t<T>>{};

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


	std::ostream& operator<<(std::ostream& out, const arena_block& block)
	{
		return out << "{" << (void*)block.arena << ": next:" << (void*)block.next << " , end:" << (void*)block.end << ", LEFT: " << (block.end - block.next) << "}";
	}

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
		ArenaAllocator(Y* pool, size_t bytes) noexcept
		{
			auto ptr = reinterpret_cast<unsigned char*>(pool);
			auto aligned_ptr = align<arena_block>(ptr);
			auto start = aligned_ptr + 1;
			new (aligned_ptr) arena_block{ start, bytes - static_cast<size_t>(reinterpret_cast<unsigned char*>(start) - ptr) };
			arena = aligned_ptr;
		}
		template<typename Y, size_t N>
		ArenaAllocator(Y(&pool)[N]) noexcept : ArenaAllocator{ pool, N * sizeof(Y) } {
		}

		template<typename A, typename = is_allocator_t<A>>
		ArenaAllocator(A && a) : arena{ a.arena }
		{
		}
		//template<typename A>
		//ArenaAllocator(A&& a){
		//    Lit<IsAllocator<::ArenaAllocator,std::remove_const_t<std::remove_reference_t<A>>>::value>();
		//    Lit<std::remove_const_t<std::remove_reference_t<A>>>();
		//};
		template<typename A, typename = is_allocator_t<A>>
		ArenaAllocator & operator=(A && a) noexcept
		{
			return arena = a.arena;
		}
		struct alignas(T) empty
		{
			char a[sizeof(T)];
		};
		ptr_t allocate_fallback(size_t n)
		{
			return (ptr_t) new empty[n];
		}
		void deallocate_fallback(ptr_t ptr)
		{
			delete[] (empty*)ptr;
		}
		ptr_t allocate(size_t n)
		{
			if (!arena)
				return allocate_fallback(n);
			ptr_t result = align<T>(arena->next);
			auto new_next = reinterpret_cast<unsigned char*>(result + n * sizeof(T));

			if (new_next > arena->end)
			{
				return allocate_fallback(n);
			}
			arena->next = new_next;
			return result;
		}
		void deallocate(ptr_t ptr, size_t N)noexcept
		{
			auto cptr = reinterpret_cast<unsigned char*>(ptr);
			if (arena && cptr >= arena->arena && cptr < arena->end)
				arena->try_release(ptr, N); // tiny optimization for stuff like stacks w/ lifo allocation
			else
				deallocate_fallback(ptr);
		}
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

	IncludeEntry FindNextIncludes(string_view& str_view, size_t& offset) noexcept
	{
		IncludeEntry result;
		string_view keyword{ "\nimport " };
		result.start = str_view.find(keyword);
		result.len = 0; //in case npos
		if (result.start != string_view::npos)
		{
			auto include_start = str_view.find_first_not_of(string_view{ " \t" }, result.start + (keyword.size() - 1));
			auto include_end = str_view.find_first_of('\n', result.start + keyword.size());
			result.include_name = string_view{ str_view.data() + include_start,include_end - include_start };
			result.len = include_end - result.start;
			//Account for the relative offset
			result.start += offset;
			//Update the running variables
			offset += include_end;
			str_view = string_view{ str_view.data() + include_end,str_view.size() - include_end };
		}
		return result;
	}
	string_view GetIncluded(string_view mounted_dir)
	{
		auto file = Core::GetSystem<FileSystem>().Open(mounted_dir, FS_PERMISSIONS::READ);
		auto st = Core::GetResourceManager().Load<ShaderSnippet>(mounted_dir,false);
		static string_view result{};
		if (st)
		{
			//Get the string from resource manager
			result = (*st)->snippet;
			//Store it into result.
		}
		return result;
	}

	template<typename T>
	using Alloc = ArenaAllocator<T>;

	using included_t =std::unordered_set<string_view, std::hash<string_view>, std::equal_to<string_view>, Alloc<string_view>>;

	void ProcessIncludesImpl(string_view glsl_code, string& final_result, included_t& included)
	{
		unsigned char pool[1 << 16];
		Alloc<char> alloc{ pool };

		std::basic_string<char, std::char_traits<char>, Alloc<char>> result{ alloc };
		vector<IncludeEntry> includes{};
		string_view find_input{ glsl_code };
		size_t offset = 0;
		while (find_input.size())
		{
			auto next = FindNextIncludes(find_input, offset);
			if (next.len == 0)
				break;
			includes.emplace_back(next);
		}
		const char* prev_end = glsl_code.data();
		for (auto& include : includes)
		{
			result.append(prev_end, glsl_code.data() + include.start);
			//If this is not a repeated include
			if (included.emplace(include.include_name).second)
			{
				//Debug annotation
				result.append("\n/* ");
				result.append(include.include_name);
				result.append("begin */\n");
				//End Debug annotation
				auto str_view = GetIncluded(include.include_name);
				result.append(str_view);
				result.append("\n/* ");
				result.append(include.include_name);
				result.append("end */\n");
			}
			else
			{
				result.append("\n/* ");
				result.append(include.include_name);
				result.append(" has already been included before. */\n");
			}
			prev_end = glsl_code.data() + include.start + include.len;//forward seek pos
		}

		//clean up the remaining bit
		result.append(prev_end, glsl_code.size()-(prev_end - glsl_code.data()));
		final_result = string{ result.data(),result.size() }; //Copy, the allocated memory must be destroyed with the arean allocator.
	}

	string ProcessIncludes(string_view glsl_code)
	{
		unsigned char pool[1 << 16];
		Alloc<char> alloc{ pool };

		string result{};
		string old_result{ glsl_code };
		included_t included{ alloc }; //Out  here to prevent recursive expansion
		while (true)
		{
			ProcessIncludesImpl(old_result, result,included); //While there's something left to expand, expand.
			if (old_result.size() == result.size())
				break;
			std::swap(old_result, result);
		}
		return result;
	}


}