#pragma once

#include <reflect/reflect.h>

namespace idk::reflect::detail
{

	template <typename T>
	struct is_pair : std::false_type
	{};
	template <typename T, typename U>
	struct is_pair<std::pair<T, U>> : std::true_type
	{};
	template <typename T>
	inline constexpr bool is_pair_v = is_pair<T>::value;



	template <typename T>
	struct is_pair_assignable : std::false_type
	{};
	template <typename T, typename U>
	struct is_pair_assignable<std::pair<T, U>> : std::true_type
	{};
	template <typename T, typename U>
	struct is_pair_assignable<std::pair<const T, U>> : std::false_type
	{};
	template <typename T, typename U>
	struct is_pair_assignable<std::pair<T, const U>> : std::false_type
	{};
	template <typename T, typename U>
	struct is_pair_assignable<std::pair<const T, const U>> : std::false_type
	{};
	template <typename T>
	inline constexpr bool is_pair_assignable_v = is_pair_assignable<T>::value;



	struct typed_context_base
	{
		const string_view name;
		const detail::table& table;
		const span<constructor_entry_base* const> ctors;
		const size_t hash;
		const bool is_container;
		const bool is_enum_type;

		typed_context_base(string_view name, const detail::table& table, span<constructor_entry_base* const> ctors, size_t hash, bool is_container, bool is_enum_type)
			: name{ name }, table{ table }, ctors{ ctors }, hash{ hash }, is_container{ is_container }, is_enum_type{ is_enum_type }
		{}
		virtual ~typed_context_base() = default;


		virtual const enum_type::data* get_enum_data() const = 0;


		virtual void copy_assign(void* lhs, const void* rhs) const = 0;
		virtual dynamic default_construct() const = 0;
		virtual dynamic copy_construct(void* obj) const = 0;

		template<typename... Ts>
		dynamic construct(Ts&& ... args) const
		{
			// inputs should be all dynamics at this stage
			static_assert(std::conjunction_v<std::is_same<Ts, dynamic>...>);

			if constexpr (sizeof...(Ts) == 0)
				return default_construct();
			else
			{
				if constexpr (sizeof...(Ts) == 1)
				{
					if (((args.type._context == this) && ...))
						return copy_construct((args._ptr->get())...);
				}

				dynamic o;
				for (auto* c : ctors)
				{
					c->try_construct(&o, args...);
					if (o.valid()) break;
				}

				//assert(o.valid());
				return o;
			}
		}
	};

	template<typename T>
	struct typed_context : typed_context_base
	{
		typed_context(string_view name, const detail::table& table, span<constructor_entry_base* const> ctors)
			: typed_context_base(name, table, ctors, typehash<T>(), is_sequential_container_v<T> || is_associative_container_v<T>, is_macro_enum_v<T>)
		{}
		typed_context()
			: typed_context(
				type_definition<T>::m_Name
				, type_definition<T>::m_Table
				, span<constructor_entry_base* const>{
						type_definition<T>::m_Storage.ctors.data(),
						type_definition<T>::m_Storage.ctors.data() + type_definition<T>::m_Storage.ctors.size() }
			)
		{}


		virtual const enum_type::data* get_enum_data() const override
		{
			if constexpr (is_macro_enum<T>::value)
			{
				constexpr static enum_type::data e{ sizeof(T::UnderlyingType), T::count, T::values, T::names };
				return &e;
			}
			else
				throw;
		}


		void copy_assign(void* lhs, const void* rhs) const override
		{
			lhs; rhs;
			if constexpr (is_pair_v<T> && !is_pair_assignable_v<T> || !std::is_copy_assignable_v<T>)
				throw "Cannot copy assign";
			else
				*static_cast<T*>(lhs) = *static_cast<const T*>(rhs);
		}

		dynamic default_construct() const override
		{
			if constexpr (!std::is_default_constructible_v<T>)
				throw "Cannot default construct";
			else
				return T{};
		}

		dynamic copy_construct(void* obj) const override
		{
			obj;
			if constexpr (!std::is_copy_constructible_v<T>)
				throw "Cannot copy construct";
			else
				return T{ *static_cast<const T*>(obj) };
		}
	};

	template<typename T>
	struct typed_context_nodef : typed_context<T>
	{
		detail::table empty_table{ 0, nullptr, nullptr, nullptr };
		typed_context_nodef()
			: typed_context<T>(
				fully_qualified_nameof<T>()
				, empty_table
				, span<constructor_entry_base* const>{ nullptr, nullptr }
			)
		{}
	};

}