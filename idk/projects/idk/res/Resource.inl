#pragma once

namespace idk
{
	namespace detail
	{
		template<typename T, size_t resID>
		struct Baseresource_finder
		{
			using check_t = std::tuple_element_t<resID, Resources>;

			using type = std::conditional_t<
				std::is_base_of_v<check_t, T>,
				check_t,
				typename Baseresource_finder<T, resID + 1>::type
			>;
		};

		template<typename T>
		struct Baseresource_finder<T, ResourceCount>
		{
			using type = void;
		};

		template<typename T>
		struct RawResourceFinder
		{
			using type = T;
		};

		template<typename T, bool is_base = index_in_tuple_v<T, Resources> != ResourceCount>
		struct ResourceFinder;

		template<typename T> struct ResourceFinder<T, false> : Baseresource_finder<T, 0> {};
		template<typename T> struct ResourceFinder<T, true> : RawResourceFinder<T> {};
	}

	template<typename Res>
	struct BaseResource
	{
		using type = typename detail::ResourceFinder<Res>::type;
	};
}