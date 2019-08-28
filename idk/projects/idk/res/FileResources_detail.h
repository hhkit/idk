#pragma once
#include <variant>
#include <tuple>

#include <idk_config.h>
#include <res/ResourceHandle.h>

namespace idk::detail
{
	template<typename T>
	struct ResourceFile_helper;

	template<typename ... Ts>
	struct ResourceFile_helper<std::tuple<Ts...>>
	{
		using GenericRscHandle = std::variant<RscHandle<Ts>...>;
	};

	template<typename Tuple, typename T, bool has_meta = has_tag_v<T, MetaTag>>
	struct ResourceMeta_impl;

	template<typename Tuple, typename T>
	struct ResourceMeta_impl<Tuple, T, true>
	{
		using type = tuple_join_t<Tuple, T>;
	};

	template<typename Tuple, typename T>
	struct ResourceMeta_impl<Tuple, T, false>
	{
		using type = Tuple;
	};
}