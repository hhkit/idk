#pragma once
#include <type_traits>

namespace idk
{
	template<typename T, template<typename ...> typename Template>
	struct is_template;

	template<typename T, template<typename ...> typename Template>
	constexpr auto is_template_v = is_template<T,Template>::value;

	template<typename Tuple, template<typename ...> typename Wrap>
	struct tuple_wrap;

	template<typename Tuple, template<typename ...> typename Wrap>
	using tuple_wrap_t = typename tuple_wrap<Tuple, Wrap>::type;

	template<typename FindMe, typename Tuple>
	struct index_in_tuple;

	template<typename FindMe, typename Tuple>
	static constexpr auto index_in_tuple_v = index_in_tuple<FindMe, Tuple>::value;

	template<typename Seq1, typename Seq2>
	struct index_sequence_cat;

	template<typename Seq1, typename Seq2>
	using index_sequence_cat_t = typename index_sequence_cat<Seq1, Seq2>::type;

	template<typename T>
	struct index_sequence_rev;

	template<typename T>
	using index_sequence_rev_t = typename index_sequence_rev<T>::type;
}

#include "meta.inl"