#pragma once
#include "vector.h"
#include <meta/casts.h>
#include <ds/range.h>
#include <ds/zip.h>

namespace idk::math::detail
{
	template<template <typename T, unsigned Sz> typename tvector, typename T, unsigned ... Indexes>
	class swizzle
	{
	public:
		static constexpr auto D = sizeof...(Indexes);
		using return_t = tvector<T, D>;

		return_t& operator=(const return_t& rhs)
		{
			//assign(rhs, std::index_sequence<Indexes...>{}, std::make_index_sequence<D>{});

			for (auto [lind, rind] : zip(std::array<unsigned, sizeof...(Indexes)>{ Indexes... }, range<sizeof...(Indexes)>()))
				values[lind] = rhs[rind];

			return *r_cast<return_t*>(this);
		}

		#define lazy_op(OP) return_t operator##OP(const return_t& rhs) const { return return_t{ *this } OP rhs; }
		lazy_op(+);
		lazy_op(-);
		lazy_op(*);
		lazy_op(/);
		#undef lazy_op
		
		operator return_t() const
		{
			return return_t{ values[Indexes]... };
		}
	private:
		T values[1];

		template<unsigned ThisFirst, unsigned ... ThisIndexes, unsigned RhsFirst, unsigned ... RhsIndexes>
		void assign(const return_t& rhs, std::index_sequence<ThisFirst, ThisIndexes...>, std::index_sequence<RhsFirst, RhsIndexes...>)
		{
			values[ThisFirst] = rhs[RhsFirst];
			assign(rhs, std::index_sequence<ThisIndexes...>{}, std::index_sequence<RhsIndexes...>{});
		}

		template<unsigned ... ThisIndexes, unsigned ... RhsIndexes>
		void assign(const return_t&, std::index_sequence<ThisIndexes...>, std::index_sequence<RhsIndexes...>)
		{
		}
	};
}