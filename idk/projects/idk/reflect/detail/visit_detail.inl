#pragma once

#include <reflect/reflect.h>

namespace idk::reflect::detail
{

	template<typename K, typename V, typename Visitor>
	void visit_key_value(K&& key, V&& val, Visitor&& visitor, int& depth, int& last_visit_depth);

	template<typename Visitor>
	void visit(void* obj, type type, Visitor&& visitor, int& depth, int& last_visit_depth)
	{
		const table& table = type._context->table;
		if (table.m_Count == 0)
			return;

		++depth;

		// adapted from EnumRecursive in properties.h#820
		for (size_t i = 0; i < table.m_Count; ++i)
		{
			auto& entry = table.m_pActionEntries[i];

			std::visit([&](auto&& FunctionGetSet) {

				using fn_getsettype = std::decay_t<decltype(FunctionGetSet)>;
				if constexpr (std::is_same_v<fn_getsettype, std::optional<std::tuple<const detail::table&, void*>>(*)(void*, std::uint64_t) noexcept>)
				{
					void* offsetted = ::property::details::HandleBasePointer(obj, entry.m_Offset);
					std::tuple<const detail::table&, void*> tup = FunctionGetSet(offsetted, 0).value();
					auto name = std::get<0>(tup).m_pName;
					void* value = std::get<1>(tup);
					visit_key_value(std::move(table.m_pEntry[i].m_pName),
									dynamic{ get_type(name), value },
									std::forward<Visitor>(visitor), depth, last_visit_depth);
					// uhhh, this branch means you're at a structure not part of idk::ReflectedTypes
					// it could also mean base class if you used REFLECT_PARENT
				}
				else
				{
					using T = ::property::vartype_from_functiongetset<fn_getsettype>;
					using DecayedT = std::decay_t<T>;

					void* offsetted = ::property::details::HandleBasePointer(obj, entry.m_Offset);
					T& value = *reinterpret_cast<T*>(offsetted);

					visit_key_value(std::move(table.m_pEntry[i].m_pName), value, std::forward<Visitor>(visitor), depth, last_visit_depth);
				}

			}, entry.m_FunctionTypeGetSet);
		}

		--depth;
	}

	template<typename Container, typename Visitor>
	void visit_container(Container& container, Visitor&& visitor, int& depth, int& last_visit_depth)
	{
		if (container.size() == 0)
			return;

		++depth;

		if constexpr (is_sequential_container_v<Container>)
		{
			size_t i = 0;
			for (auto& elem : container)
			{
				// key should be rvalue, so move it
				visit_key_value(std::move(i), elem, std::forward<Visitor>(visitor), depth, last_visit_depth);
				++i;
			}
		}
		else if constexpr (is_associative_container_v<Container>)
		{
			using V = decltype(*container.begin());

			for (auto& elem : container)
			{
				if constexpr (is_template_v<std::decay_t<V>, std::pair>)
					visit_key_value(elem.first, elem.second, std::forward<Visitor>(visitor), depth, last_visit_depth);
				else
					visit_key_value(elem, elem, std::forward<Visitor>(visitor), depth, last_visit_depth);
			}
		}

		--depth;
	}

	template<typename Visitor, typename... Ts>
	void visit_variant(variant<Ts...>& var, Visitor&& visitor, int& depth, int& last_visit_depth)
	{
		++depth;

#define VISIT_INDEX(I) case I: visit_key_value(get_type<std::variant_alternative_t<I, variant<Ts...>>>(), std::get<I>(var), std::forward<Visitor>(visitor), depth, last_visit_depth); break;
#define VARIANT_CASE(N, ...) if constexpr (sizeof...(Ts) == N) { switch (var.index()) { IDENTITY(FOREACH(VISIT_INDEX, __VA_ARGS__)) default: throw "Unhandled case?"; } }
		VARIANT_CASE(1, 0)
else VARIANT_CASE(1, 0)
			else VARIANT_CASE(2, 1, 0)
			else VARIANT_CASE(3, 2, 1, 0)
			else VARIANT_CASE(4, 3, 2, 1, 0)
			else VARIANT_CASE(5, 4, 3, 2, 1, 0)
			else VARIANT_CASE(6, 5, 4, 3, 2, 1, 0)
			else VARIANT_CASE(7, 6, 5, 4, 3, 2, 1, 0)
			else VARIANT_CASE(8, 7, 6, 5, 4, 3, 2, 1, 0)
			else VARIANT_CASE(9, 8, 7, 6, 5, 4, 3, 2, 1, 0)
			else VARIANT_CASE(10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)
			else throw "Unhandled case?";
#undef VISIT_INDEX
#undef VARIANT_CASE

			--depth;
	}

	// where visitor is actually called
	template<typename K, typename V, typename Visitor>
	void visit_key_value(K&& key, V&& val, Visitor&& visitor, int& depth, int& last_visit_depth)
	{
		constexpr static bool ValIsContainer =
			(is_associative_container_v<V> || is_sequential_container_v<V>) && !std::is_same_v<std::decay_t<V>, string>;
		constexpr static bool ValIsVariant =
			is_template_v<std::decay_t<V>, std::variant>;
		constexpr static bool ValIsDynamic =
			std::is_same_v<std::decay_t<V>, dynamic>;

		int depth_change = depth - last_visit_depth;
		last_visit_depth = depth;

		if constexpr (std::is_same_v<std::result_of_t<decltype(visitor)(K, V, int&)>, bool>) // does it return bool?
		{
			if (visitor(std::forward<K>(key), std::forward<V>(val), depth_change)) // if false, stop recursive
			{
				if constexpr (ValIsDynamic)
					visit(val._ptr->get(), val.type, std::forward<Visitor>(visitor), depth, last_visit_depth);
				else if constexpr (ValIsContainer)
					visit_container(val, std::forward<Visitor>(visitor), depth, last_visit_depth);
				else if constexpr (ValIsVariant)
					visit_variant(val, std::forward<Visitor>(visitor), depth, last_visit_depth);
				else
					visit(&val, get_type<decltype(val)>(), std::forward<Visitor>(visitor), depth, last_visit_depth);
			}
		}
		else // does not return bool, always stop recursive
		{
			visitor(std::forward<K>(key), std::forward<V>(val), depth_change);
			if constexpr (ValIsDynamic)
				visit(val._ptr->get(), val.type, std::forward<Visitor>(visitor), depth, last_visit_depth);
			else if constexpr (ValIsContainer)
				visit_container(val, std::forward<Visitor>(visitor), depth, last_visit_depth);
			else if constexpr (ValIsVariant)
				visit_variant(val, std::forward<Visitor>(visitor), depth, last_visit_depth);
			else
				visit(&val, get_type<decltype(val)>(), std::forward<Visitor>(visitor), depth, last_visit_depth);
		}
	}

}