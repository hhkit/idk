#include "stdafx.h"
#include "ShaderIncluder.h"
namespace idk
{
	namespace detail
	{

		template<template<typename>typename TM, typename T>
		struct IsAllocator : std::false_type
		{

		};

		template<template<typename>typename TM, typename T>
		struct IsAllocator<TM, TM<T>> :std::true_type
		{

		};

	}

	template<template<typename>typename TM, typename T>
	struct IsAllocator : detail::IsAllocator<TM, std::decay_t<T>>
	{

	};

	//MUST BE POD, TRIVIAL DESTRUCTOR.
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
		return reinterpret_cast<T*>(ptr + (mod)? align_of-mod:0);
	}


	template<typename T>
	struct ArenaAllocator
	{
		using ptr_t = T *;
		using value_type = T;

		template<typename A>
		using is_allocator_t = std::enable_if_t<IsAllocator<idk::ArenaAllocator, std::remove_const_t<std::remove_reference_t<A>>>::value>;
		arena_block* arena;

		template<typename Y>
		ArenaAllocator(Y* pool, size_t bytes) noexcept
		{
			auto ptr = reinterpret_cast<unsigned char*>(pool);
			auto aligned_ptr = align<arena_block>(ptr);
			auto start = aligned_ptr + 1;
			new (aligned_ptr) arena_block{ start, bytes - static_cast<size_t>(reinterpret_cast<unsigned char*>(start) - ptr) };
		}
		template<typename Y,size_t N>
		ArenaAllocator(Y(&pool)[N]) noexcept : ArenaAllocator{ pool, N } {}

		template<typename A, typename = is_allocator_t<A>>
		ArenaAllocator(A && a) : arena{ a.arena } {}
		//template<typename A>
		//ArenaAllocator(A&& a){
		//    Lit<IsAllocator<::ArenaAllocator,std::remove_const_t<std::remove_reference_t<A>>>::value>();
		//    Lit<std::remove_const_t<std::remove_reference_t<A>>>();
		//};
		template<typename A, typename = is_allocator_t<A>>
		ArenaAllocator& operator=(A&& a) noexcept
		{
			return arena = a.arena;
		}
		struct alignas(T) empty
		{
			char a[sizeof(T)];
		};
		ptr_t allocate(size_t n)
		{
			ptr_t result = align<T>(arena->next);
			arena->next = reinterpret_cast<unsigned char*>(result + n * sizeof(result));
			if (arena->next > arena->end)
				throw std::bad_alloc{};
			return result;
		}
		void deallocate(ptr_t ptr, size_t N)noexcept
		{
			arena->try_release(ptr, N);
			//do nothing
		}
		template<typename A, typename = is_allocator_t<A>>
		bool operator==(const A& rhs)const noexcept
		{
			return arena==rhs.arena;
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
			auto include_start = str_view.find_first_not_of(' ', result.start + keyword.size());
			auto include_end = str_view.find_first_of('\n', result.start);
			result.include_name = string_view{ str_view.data() + include_start,include_end - include_start };
			result.len = include_end - include_start;
			//Account for the relative offset
			result.start += offset;
			//Update the running variables
			offset += result.len;
			str_view = string_view{ str_view.data() + include_end,str_view.size() - include_end };
		}
		return result;
	}

	string_view GetIncluded(string_view mounted_dir)
	{
		auto st = Core::GetResourceManager().Get<ShaderSnippet>(mounted_dir);
		string_view result{};
		if (st)
		{
			//Get the string from resource manager
			result = st->as<ShaderSnippet>().snippet;
			//Store it into result.
		}
		return result;
	}

	string ProcessIncludes(string_view glsl_code)
	{
		unsigned char pool[1 << 16];
		ArenaAllocator<char> alloc{ pool };
		std::basic_string<char, std::char_traits<char>, ArenaAllocator<char>> result{ alloc };
		std::unordered_set<string_view, std::hash<string_view>, std::equal_to<string_view>, ArenaAllocator<string_view>> included{ alloc };
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
				result.append("/*");
				result.append(include.include_name);
				result.append("*/\n");
				//End Debug annotation
				result.append(GetIncluded(include.include_name));
			}
			prev_end += include.len;//forward seek pos
		}
		//clean up the remaining bit
		result.append(prev_end, (prev_end - glsl_code.data()) + glsl_code.size());
		string final_result{ result.data(),result.size() }; //Copy, the allocated memory must be destroyed with the arean allocator.
		return final_result;
	}

}