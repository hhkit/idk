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

		template<typename T>
		register_type<T>::register_type()
		{
			if constexpr (is_defined_v<type_definition<T>>)
			{
				static typed_context<T> context{};
				meta::instance().names_to_contexts.emplace(type_definition<T>::m_Name, &context);
				meta::instance().hashes_to_contexts.emplace(typehash<T>(), &context);
			}
			else
			{
				static typed_context_nodef<T> context{};
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
			static detail::register_type<std::decay_t<T>> __reg;
			return get_type<T>();
		}
		return type{ iter->second };
	}



	template<typename T, typename Visitor>
	void visit(T& obj, Visitor&& visitor)
	{
		detail::visit(&obj, get_type<T>()._context->table, std::forward<Visitor>(visitor));
	}

	namespace detail
	{
		template<typename Visitor>
		void visit(void* obj, const detail::table& table, Visitor&& visitor)
		{
			if (table.m_Count == 0)
				return;

			// adapted from EnumRecursive in properties.h#820
			for (size_t i = 0; i < table.m_Count; ++i)
			{
				auto& Entry = table.m_pActionEntries[i];

				std::visit([&](auto&& FunctionGetSet) {

					using fn_getsettype = std::decay_t<decltype(FunctionGetSet)>;
					if constexpr (std::is_same_v<fn_getsettype, std::optional<std::tuple<const detail::table&, void*>>(*)(void*, std::uint64_t) noexcept>)
					{
						throw; // uhhh, this branch means you're at a structure not part of idk::ReflectedTypes
					}
					else
					{
						using T = ::property::vartype_from_functiongetset<fn_getsettype>;
						void* offsetted = ::property::details::HandleBasePointer(obj, Entry.m_Offset);
						T& value = *reinterpret_cast<T*>(offsetted);

						if constexpr (std::is_same_v<std::result_of_t<decltype(visitor)(const char*, T&)>, bool>)
						{
							if (visitor(table.m_pEntry[i].m_pName, value)) // if false, stop recursive
								reflect::visit(value, std::forward<Visitor>(visitor));
						}
						else // function has no return
						{
							visitor(table.m_pEntry[i].m_pName, value);
							reflect::visit(value, std::forward<Visitor>(visitor));
						}
					}

				}, table.m_pActionEntries[i].m_FunctionTypeGetSet);
			}
		}
	}

}