#pragma once

#include <reflect/reflect.h>

namespace idk::reflect::detail
{
	// replacement for property::table_storage
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