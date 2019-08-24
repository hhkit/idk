#pragma once

#include "reflect.h"

#include "typed_context.inl"
#include "constructor_entry.inl"
#include "type.inl"
#include "dynamic.inl"
#include "uni_container.inl"
#include "enum_type.inl"

#pragma warning (disable: 4996) // deprecation warning for std::result_of

namespace idk::reflect
{

	namespace detail
	{

		template<typename T, bool HasTypeDefinition>
		register_type<T, HasTypeDefinition>::register_type()
		{
			if constexpr (HasTypeDefinition)
			{
				static typed_context<T> context{};
				meta::instance().names_to_contexts.emplace(type_definition<T>::m_Name, &context);
				meta::instance().hashes_to_contexts.emplace(typehash<T>(), &context);
			}
			else
			{
				static typed_context_nodef<T> context{};
				meta::instance().names_to_contexts.emplace(context.name, &context);
				meta::instance().hashes_to_contexts.emplace(typehash<T>(), &context);
			}
		}



		template<typename ClassT, typename... Ts>
		struct table_storage
		{
			constexpr static auto entry_count_v =
				std::tuple_size_v<tuple_cat_t<std::conditional_t<is_template_v<Ts, constructor_entry>, std::tuple<>, std::tuple<Ts>>...>>;

			std::array<::property::table_action_entry, entry_count_v> m_ActionEntry;
			std::array<::property::table_entry, entry_count_v> m_UserEntry;

			std::array<constructor_entry_base*, sizeof...(Ts) - entry_count_v + std::is_default_constructible_v<ClassT>> ctors;
			int i = 0;
			int j = 0;

			table_storage(class_holder<ClassT>, Ts&& ... args)
			{
				if constexpr (std::is_default_constructible_v<ClassT>)
				{
					static constructor_entry<ClassT> ctor{};
					ctors[j++] = &ctor;
				}
				(assign(std::forward<Ts>(args)), ...);
			}

			template<typename T>
			void assign(T&& arg)
			{
				if constexpr (is_template_v<T, constructor_entry>)
				{
					static T ctor = std::forward<T>(arg);
					ctors[j++] = &ctor;
				}
				else
				{
					m_ActionEntry[i] = std::forward<T>(arg);
					m_UserEntry[i++] = std::forward<T>(arg);
				}
			}
		};
	}



	template<typename T>
	type get_type()
	{
		auto iter = detail::meta::instance().hashes_to_contexts.find(typehash<T>());
		if (iter == detail::meta::instance().hashes_to_contexts.end()) // type not registered?
		{
			detail::register_type<std::decay_t<T>, false> __reg;
			return get_type<T>();
		}
		return type{ iter->second };
	}



	// recursively visit all members of an object
	// visitor must be a function with signature:
	//  (auto&& key, auto&& value, int depth_change) -> bool/void
	// 
	// key:
	//     name of property (const char*), or
	//     container key when visiting container elements ( K = std::decay_t<decltype(key)> )
	//     for sequential containers, it will be size_t. for associative, it will be type K
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
		detail::visit(&obj, get_type<T>(), std::forward<Visitor>(visitor), depth);
	}

	namespace detail
	{
		template<typename K, typename V, typename Visitor>
		void visit_key_value(K&& key, V&& val, Visitor&& visitor, int& depth, int& curr_depth);

		template<typename Visitor>
		void visit(void* obj, type type, Visitor&& visitor, int& depth)
		{
			const table& table = type._context->table;
			if (table.m_Count == 0)
				return;

			int curr_depth = depth;
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
							std::forward<Visitor>(visitor), depth, curr_depth);
						// uhhh, this branch means you're at a structure not part of idk::ReflectedTypes
						// it could also mean base class if you used REFLECT_PARENT
					}
					else
					{
						using T = ::property::vartype_from_functiongetset<fn_getsettype>;
						using DecayedT = std::decay_t<T>;

						void* offsetted = ::property::details::HandleBasePointer(obj, entry.m_Offset);
						T& value = *reinterpret_cast<T*>(offsetted);

						visit_key_value(std::move(table.m_pEntry[i].m_pName), value, std::forward<Visitor>(visitor), depth, curr_depth);
					}

				}, entry.m_FunctionTypeGetSet);
			}
		}

		template<typename Container, typename Visitor>
		void visit_container(Container& container, Visitor&& visitor, int& depth)
		{
			if (container.size() == 0)
				return;

			int curr_depth = depth;
			++depth;

			if constexpr (is_sequential_container_v<Container>)
			{
				size_t i = 0;
				for (auto& elem : container)
				{
					// key should be rvalue, so move it
					visit_key_value(std::move(i), elem, std::forward<Visitor>(visitor), depth, curr_depth);
					++i;
				}
			}
			else if constexpr (is_associative_container_v<Container>)
			{
				using V = decltype(*container.begin());

				for (auto& elem : container)
				{
					if constexpr (is_template_v<std::decay_t<V>, std::pair>)
						visit_key_value(elem.first, elem.second, std::forward<Visitor>(visitor), depth, curr_depth);
					else
						visit_key_value(elem, elem, std::forward<Visitor>(visitor), depth, curr_depth);
				}
			}
		}

		template<typename Visitor, typename... Ts>
		void visit_variant(variant<Ts...>& var, Visitor&& visitor, int& depth)
		{
			int curr_depth = depth;
			++depth;

#define VISIT_INDEX(I) case I: visit_key_value(I, std::get<I>(var), std::forward<Visitor>(visitor), depth, curr_depth); return;
#define VARIANT_CASE(N, ...) if constexpr (sizeof...(Ts) == N) { switch (var.index()) { IDENTITY(FOREACH(VISIT_INDEX, __VA_ARGS__)) } }
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
#undef VISIT_INDEX
#undef VARIANT_CASE
		}

		// where visitor is actually called
		template<typename K, typename V, typename Visitor>
		void visit_key_value(K&& key, V&& val, Visitor&& visitor, int& depth, int& curr_depth)
		{
			constexpr bool ValIsContainer =
				(is_associative_container_v<V> || is_sequential_container_v<V>) && !std::is_same_v<std::decay_t<V>, string>;
			constexpr bool ValIsVariant =
				is_template_v<std::decay_t<V>, std::variant>;
			constexpr bool ValIsDynamic =
				std::is_same_v<std::decay_t<V>, dynamic>;
			int depth_change = depth - curr_depth;
			curr_depth = depth;

			if constexpr (std::is_same_v<std::result_of_t<decltype(visitor)(K, V, int&)>, bool>) // does it return bool?
			{
				if (visitor(std::forward<K>(key), std::forward<V>(val), depth_change)) // if false, stop recursive
				{
					if constexpr (ValIsDynamic)
						visit(*static_cast<void**>(val._ptr->get()), val.type, std::forward<Visitor>(visitor), depth);
					else if constexpr (ValIsContainer)
						visit_container(val, std::forward<Visitor>(visitor), depth);
					else if constexpr(ValIsVariant)
						visit_variant(val, std::forward<Visitor>(visitor), depth);
					else
						visit(&val, get_type<decltype(val)>(), std::forward<Visitor>(visitor), depth);
					std::swap(depth, curr_depth);
				}
			}
			else
			{
				visitor(std::forward<K>(key), std::forward<V>(val), depth_change);
				if constexpr (ValIsDynamic)
					visit(*static_cast<void**>(val._ptr->get()), val.type, std::forward<Visitor>(visitor), depth);
				else if constexpr (ValIsContainer)
					visit_container(val, std::forward<Visitor>(visitor), depth);
				else if constexpr (ValIsVariant)
					visit_variant(val, std::forward<Visitor>(visitor), depth);
				else
					visit(&val, get_type<decltype(val)>(), std::forward<Visitor>(visitor), depth);
				std::swap(depth, curr_depth);
			}
		}
	}

}