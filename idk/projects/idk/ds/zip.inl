#pragma once
#include <utility>
#include <tuple>

namespace idk
{
	template<typename ... Ts>
	struct zipped
	{
		struct iterator_t
		{
			using intern_t = std::tuple<decltype(std::begin(std::declval<Ts&&>()))...>;
			intern_t track;
			static constexpr auto indexes = std::make_index_sequence < std::tuple_size_v<intern_t>>{};

			template<unsigned ... Indexes>
			constexpr void _advance(std::index_sequence<Indexes...>) noexcept
			{
				const auto var = std::make_tuple(++std::get<Indexes>(track) ...);
				(var);
			}

			template<unsigned ... Indexes>
			constexpr auto _deref(std::index_sequence<Indexes...>) noexcept
			{
				return std::tie(*std::get<Indexes>(track)...);
			}

			template<unsigned ... Indexes>
			constexpr auto _ptr(std::index_sequence<Indexes...>) noexcept
			{
				return std::make_tuple(std::get<Indexes>(track).operator->()...);
			}

			template<unsigned ... Indexes>
			constexpr auto _equal(const iterator_t& rhs, std::index_sequence<Indexes...>) const noexcept
			{
				return ((std::get<Indexes>(track) == std::get<Indexes>(rhs.track)) || ...);
			}

			constexpr iterator_t& operator++() noexcept
			{
				_advance(indexes);
				return *this;
			}

			constexpr auto operator*() noexcept
			{
				return _deref(indexes);
			}

			constexpr bool operator!=(const iterator_t& rhs) const noexcept
			{
				return !_equal(rhs, indexes);
			}

			constexpr bool operator==(const iterator_t& rhs) const noexcept
			{
				return _equal(rhs, indexes);
			}
		};
		std::tuple<Ts...> containers;
		iterator_t _begin;
		iterator_t _end;

		template<size_t ... Indexes>
		constexpr zipped(std::index_sequence<Indexes...>, Ts&& ... _containers)
			: containers{ std::forward<Ts>(_containers)... },
			_begin{ std::make_tuple(std::begin(std::get<Indexes>(containers))...) },
			_end{ std::make_tuple(std::end(std::get<Indexes>(containers))...) }
		{

		}


		constexpr zipped(Ts&& ... _containers) noexcept
			:zipped{ std::make_index_sequence<sizeof...(Ts)>{}, std::forward<Ts>(_containers)... }
		{}
		
		constexpr iterator_t begin() const noexcept
		{
			return _begin;
		}

		constexpr iterator_t end() const noexcept
		{
			return _end;
		}
	};


	template<typename ... Containers>
	constexpr auto zip(Containers&& ... containers) noexcept
	{
		return zipped<Containers...>{ std::forward<Containers>(containers) ...};
	}
}