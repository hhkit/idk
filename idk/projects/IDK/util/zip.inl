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
			using intern_t = std::tuple<decltype(std::declval<Ts>().begin())...>;
			intern_t track;
			static constexpr auto indexes = std::make_index_sequence < std::tuple_size_v<intern_t>>{};

			template<unsigned ... Indexes>
			void _advance(std::index_sequence<Indexes...>)
			{
				auto var = std::make_tuple(++std::get<Indexes>(track) ...);
				(var);
			}

			template<unsigned ... Indexes>
			auto _deref(std::index_sequence<Indexes...>)
			{
				return std::tie(*std::get<Indexes>(track)...);
			}

			template<unsigned ... Indexes>
			auto _ptr(std::index_sequence<Indexes...>)
			{
				return std::make_tuple(std::get<Indexes>(track).operator->()...);
			}

			template<unsigned ... Indexes>
			auto _equal(const iterator_t& rhs, std::index_sequence<Indexes...>) const
			{
				return ((std::get<Indexes>(track) == std::get<Indexes>(rhs.track)) || ...);
			}

			iterator_t& operator++()
			{
				_advance(indexes);
				return *this;
			}

			auto operator*()
			{
				return _deref(indexes);
			}

			bool operator!=(const iterator_t& rhs) const
			{
				return !_equal(rhs, indexes);
			}

			bool operator==(const iterator_t& rhs) const
			{
				return _equal(rhs, indexes);
			}
		};
		std::tuple<Ts...> containers;
		iterator_t _begin;
		iterator_t _end;

		zipped(Ts&& ... containers)
			: containers{ std::forward<Ts>(containers)... }, 
			_begin {std::make_tuple(containers.begin()...)},
			_end{std::make_tuple(containers.end()...)}
		{}
		
		iterator_t begin()
		{
			return _begin;
		}

		iterator_t end()
		{
			return _end;
		}
	};


	template<typename ... Containers>
	auto zip(Containers&& ... containers)
	{
		return zipped<Containers...>{ std::forward<Containers>(containers) ...};
	}
}