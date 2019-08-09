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
			using intern_t = std::tuple<decltype(std::begin(std::declval<Ts&>()))...>;
			intern_t track;
			static constexpr auto indexes = std::make_index_sequence < std::tuple_size_v<intern_t>>{};

			template<unsigned ... Indexes>
			constexpr void _advance(std::index_sequence<Indexes...>)
			{
				auto var = std::make_tuple(++std::get<Indexes>(track) ...);
				(var);
			}

			template<unsigned ... Indexes>
			constexpr auto _deref(std::index_sequence<Indexes...>)
			{
				return std::tie(*std::get<Indexes>(track)...);
			}

			template<unsigned ... Indexes>
			constexpr auto _ptr(std::index_sequence<Indexes...>)
			{
				return std::make_tuple(std::get<Indexes>(track).operator->()...);
			}

			template<unsigned ... Indexes>
			constexpr auto _equal(const iterator_t& rhs, std::index_sequence<Indexes...>) const
			{
				return ((std::get<Indexes>(track) == std::get<Indexes>(rhs.track)) || ...);
			}

			constexpr iterator_t& operator++()
			{
				_advance(indexes);
				return *this;
			}

			constexpr auto operator*()
			{
				return _deref(indexes);
			}

			constexpr bool operator!=(const iterator_t& rhs) const
			{
				return !_equal(rhs, indexes);
			}

			constexpr bool operator==(const iterator_t& rhs) const
			{
				return _equal(rhs, indexes);
			}
		};
		iterator_t _begin;
		iterator_t _end;

		constexpr zipped(Ts& ... containers) noexcept
			: _begin {std::make_tuple(std::begin(containers)...)},
			_end{std::make_tuple(std::end(containers)...)}
		{}

		constexpr iterator_t begin() noexcept
		{
			return _begin;
		}

		constexpr iterator_t end() noexcept
		{
			return _end;
		}

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
	constexpr auto zip(Containers& ... containers) noexcept
	{
		return zipped<Containers...>{ containers ...};
	}
}