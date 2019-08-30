#pragma once
// tuple utils

#include <tuple>

namespace idk
{
	template<typename T, template<typename ...> typename Template>
	struct is_template;

	template<typename T, template<typename ...> typename Template>
	constexpr auto is_template_v = is_template<T, Template>::value;


	template<typename Tuple, template<typename ...> typename Wrap>
	struct tuple_wrap;

	template<typename Tuple, template<typename ...> typename Wrap>
	using tuple_wrap_t = typename tuple_wrap<Tuple, Wrap>::type;


	template<typename...Ts>
	using tuple_cat_t = decltype(std::tuple_cat(std::declval<Ts>()...));

	template<typename T1, typename T2>
	struct tuple_join;

	template<typename T1, typename T2>
	using tuple_join_t = typename tuple_join<T1, T2>::type;
	
	template<typename FindMe, typename Tuple>
	struct index_in_tuple;

	template<typename FindMe, typename Tuple>
	static constexpr auto index_in_tuple_v = index_in_tuple<FindMe, Tuple>::value;

	template<typename T, typename ... Args>
	constexpr auto tuple_construct(const std::tuple<Args...>&) noexcept;
}
#include "tuple.inl"