#pragma once
#pragma warning (disable: 4996) // deprecation warning for std::result_of

#include "reflect.h"

namespace idk::reflect::detail
{
	using table = ::property::table;
	template<typename T> using type_definition = ::property::opin::def<T>;
}

#include "detail/typed_context.inl"
#include "detail/constructor_entry.inl"
#include "detail/meta_manager.inl"
#include "detail/table_storage.inl"
#include "detail/visit_detail.inl"

#include "type.inl"
#include "dynamic.inl"
#include "uni_container.inl"
#include "enum_type.inl"

namespace idk::reflect
{
	template<typename T>
	type get_type()
	{
		auto iter = detail::meta::instance().hashes_to_contexts.find(typehash<T>());
		if (iter == detail::meta::instance().hashes_to_contexts.end()) // type not registered?
		{
			const detail::register_type<std::decay_t<T>, false> __reg;
			return get_type<T>();
		}
		return type{ iter->second };
	}

    namespace detail
    {
        template<typename T>
        struct is_unpackable : false_type {};
        template<template<typename...> typename Tpl, typename... Ts>
        struct is_unpackable<Tpl<Ts...>> : true_type {};

        template<typename T, size_t I>
        struct type_at_index;
        template<size_t I, template<typename...> typename Tpl, typename... Ts>
        struct type_at_index<Tpl<Ts...>, I> { using type = std::tuple_element_t<I, std::tuple<Ts...>>; };

        template<typename T>
        struct pack_size;
        template<template<typename...> typename Tpl, typename... Ts>
        struct pack_size<Tpl<Ts...>> { constexpr static auto value = sizeof...(Ts); };

        template<typename T, size_t... Is>
        span<type> unpack_types(std::index_sequence<Is...>)
        {
            static array<type, sizeof...(Is)> types{ get_type<typename type_at_index<T, Is>::type>()... };
            return span<type>(types);
        }
    }

    // get span of types of packed T, such as tuple or variant
    // usage example: reflect::unpack_types< variant<int, float, vec3> >()
    template<typename T, typename = sfinae<detail::is_unpackable<T>::value>>
    span<type> unpack_types()
    {
        return detail::unpack_types<T>(std::make_index_sequence<detail::pack_size<T>::value>());
    }

	// recursively visit all members of an object
	// visitor must be a function with signature:
	//  (auto&& key, auto&& value, int depth_change) -> bool/void
	// 
	// key:
	//     name of property (const char*), or
	//     container key when visiting container elements ( K = std::decay_t<decltype(key)> )
	//     for sequential containers, it will be size_t. for associative, it will be type K
	//     for held object of variants, it will be the held type (as a reflect::type)
	// value:
	//     the value, use T = std::decay_t<decltype(value)> to get the type
	// depth_change: (int)
	//     change in depth. -1 (up a level), 0 (stay same level), or 1 (down a level)
	// 
	// return false to stop recursion. if function doesn't return, it always recurses
	template<typename T, typename Visitor>
	void visit(T& obj, Visitor&& visitor)
	{
		int depth = 0;
		int last_visit_depth = 0;
		detail::visit(&obj, get_type<T>(), std::forward<Visitor>(visitor), depth, last_visit_depth);
	}
}