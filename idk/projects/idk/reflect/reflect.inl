#pragma once

#include "reflect.h"

#include "typed_context.inl"
#include "constructor_entry.inl"
#include "type.inl"
#include "dynamic.inl"

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
	//  (const char* name, auto&& data, int depth_change) -> bool/void
	// name: name of property
	// data: the value, use T = std::decay_t<decltype(data)> to get the type
	// depth_change: the change in depth; -1, 0, or 1. (1 means down a level)
	// return false to stop recursion. if function doesn't return, it always recurses
	template<typename T, typename Visitor>
	void visit(T& obj, Visitor&& visitor)
	{
		int depth = 0;
		detail::visit(&obj, get_type<T>(), std::forward<Visitor>(visitor), depth);
	}

	namespace detail
	{
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
					int depth_change = depth - curr_depth;
					curr_depth = depth;

					if constexpr (std::is_same_v<std::result_of_t<decltype(visitor)(size_t, decltype(elem), int&)>, bool>) // does it return bool?
					{
						if (visitor(i, elem, depth_change)) // if false, stop recursive
						{
							if constexpr (is_iterable_v<decltype(elem)>)
								visit_container(elem, std::forward<Visitor>(visitor), depth);
							else
								visit(&elem, get_type<decltype(elem)>(), std::forward<Visitor>(visitor), depth);
							std::swap(depth, curr_depth);
						}
					}
					else
					{
						visitor(i, elem, depth_change);
						if constexpr (is_iterable_v<decltype(elem)> && !std::is_same_v<std::decay_t<decltype(elem)>, string>)
							visit_container(elem, std::forward<Visitor>(visitor), depth);
						else
							visit(&elem, get_type<decltype(elem)>(), std::forward<Visitor>(visitor), depth);
						std::swap(depth, curr_depth);
					}

					++i;
				}
			}
		}

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
						throw; // uhhh, this branch means you're at a structure not part of idk::ReflectedTypes
					}
					else
					{
						using T = ::property::vartype_from_functiongetset<fn_getsettype>;
						using DecayedT = std::decay_t<T>;

						void* offsetted = ::property::details::HandleBasePointer(obj, entry.m_Offset);
						T& value = *reinterpret_cast<T*>(offsetted);

						int depth_change = depth - curr_depth;
						curr_depth = depth;

						if constexpr (std::is_same_v<std::result_of_t<decltype(visitor)(const char*, T&, int&)>, bool>)
						{
							if (visitor(table.m_pEntry[i].m_pName, value, depth_change)) // if false, stop recursive
							{
								if constexpr (is_sequential_container_v<DecayedT>)
									visit_container(value, std::forward<Visitor>(visitor), depth);
								else
									visit(offsetted, get_type<T>(), std::forward<Visitor>(visitor), depth);
								std::swap(depth, curr_depth);
							}
						}
						else // function has no return
						{
							visitor(table.m_pEntry[i].m_pName, value, depth_change);
							if constexpr (is_sequential_container_v<DecayedT>)
								visit_container(value, std::forward<Visitor>(visitor), depth);
							else
								visit(offsetted, get_type<T>(), std::forward<Visitor>(visitor), depth);
							std::swap(depth, curr_depth);
						}
					}

				}, entry.m_FunctionTypeGetSet);
			}
		}
	}

}