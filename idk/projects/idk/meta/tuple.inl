#pragma once
#include <utility>

namespace idk
{
	namespace detail
	{
		template<typename T, typename ... Args, unsigned ... Indexes>
		auto tuple_construct_helper(const std::tuple<Args...>& arg_tuple, std::index_sequence<Indexes...>)
		{
			return T{std::forward<Args>(std::get<Indexes>(arg_tuple))...};
		}
	}
	template<typename T, typename ... Args>
	constexpr auto tuple_construct(const std::tuple<Args...>& arg_tuple) noexcept
	{
		return detail::tuple_construct_helper<T>(arg_tuple, std::make_index_sequence < std::tuple_size_v<std::decay_t<decltype(arg_tuple)> >> {});
	}

	template<typename ... T1s, typename ...T2s>
	struct tuple_join<std::tuple<T1s...>, std::tuple<T2s...>>
	{
		using type = std::tuple<T1s..., T2s...>;
	};

    template<typename ... Ts, template<typename ...> typename Wrap>
    struct tuple_wrap<std::tuple<Ts...>, Wrap>
    {
        using type = std::tuple<Wrap<Ts>...>;
    };

    template<typename FindMe>
    struct index_in_tuple<FindMe, std::tuple<>>
    {
        static constexpr uint8_t value = 0;
    };

    template<typename FindMe, typename ... Ts>
    struct index_in_tuple<FindMe, std::tuple<FindMe, Ts...>>
    {
        static constexpr uint8_t value = 0;
    };

    template<typename FindMe, typename First, typename ... Ts>
    struct index_in_tuple<FindMe, std::tuple<First, Ts...>>
    {
        static constexpr uint8_t value = index_in_tuple<FindMe, std::tuple<Ts...>>::value + 1;
    };
}