#pragma once
#include "comparator.h"


namespace idk
{

	namespace detail
	{

		template<typename Ptr>
		struct ptr_mem_info;
		template<typename C, typename T>
		struct ptr_mem_info<T(C::*)>
		{
			using class_t = C;
			using type = T;
		};
		template<typename ptr>
		using ptr_mem_info_type = typename ptr_mem_info<ptr>::type;
		template<typename ptr>
		using ptr_mem_info_class = typename ptr_mem_info<ptr>::class_t;


		struct poss
		{
			static constexpr int yes = 0, no = 1, maybe = -1;
			int val = maybe;
			inline poss operator|(const poss& rhs)const noexcept
			{
				int nval = rhs.val;
				nval = val == maybe ? nval : val;
				return poss{ nval };
			}
			inline operator bool()const noexcept
			{
				return (val == yes);
			}
		};

		template<typename curr_t, typename T>
		inline poss compare(const T& lhs, const T& rhs) noexcept
		{
			//using curr_t = typename Index_<N,tuple>::type;
			int v = curr_t::get(lhs) < curr_t::get(rhs) ? poss::yes : poss::no;
			v = curr_t::get(lhs) != curr_t::get(rhs) ? v : poss::maybe;
			return poss{ v };
		}
		template<typename ...Args>
		bool or_together(Args... args) noexcept
		{
			return static_cast<bool>((args | ...));
		}

	}
	template<typename Ptr, Ptr ptr>
	struct PtrMem
	{
		using T = detail::ptr_mem_info_type <Ptr>;
		using C = detail::ptr_mem_info_class<Ptr>;
		constexpr static Ptr pointer = ptr;
		static const T& get(const C& obj) noexcept
		{
			return (obj.*pointer);
		}
	};

	template<typename T, typename...Args>
	bool ordered_comparator<T, Args...>::operator()(const T& lhs, const T& rhs)const noexcept
	{
		return detail::or_together(detail::compare<Args>(lhs, rhs)...);
	}
}