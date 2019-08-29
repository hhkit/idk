#pragma once

#include <reflect/reflect.h>

namespace idk::reflect::detail
{

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
		virtual void variant_assign(void* lhs, const ReflectedTypes& rhs) const = 0;
		virtual ReflectedTypes get_mega_variant(void* obj) const = 0;
		virtual dynamic default_construct() const = 0;
		virtual dynamic copy_construct(void* obj) const = 0;
		virtual uni_container to_container(void* obj) const = 0;
		virtual enum_value to_enum_value(void* obj) const = 0;
		virtual vector<dynamic> unpack(void* obj) const = 0;
		virtual dynamic get_variant_value(void* obj) const = 0;

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
			if constexpr (is_template_v<T, std::pair> && !is_pair_assignable_v<T> || !std::is_copy_assignable_v<T>)
				throw "Cannot copy assign";
			else
				*static_cast<T*>(lhs) = *static_cast<const T*>(rhs);
		}

		virtual void variant_assign(void* lhs, const ReflectedTypes& rhs) const override
		{
			std::visit([lhs](auto&& arg)
			{
				if constexpr (std::is_convertible_v<decltype(arg), T>)
				{
					if constexpr (std::is_arithmetic_v<std::decay_t<T>>)
						*static_cast<T*>(lhs) = static_cast<T>(arg);
					else
						*static_cast<T*>(lhs) = arg;
				}
				else
					throw "Cannot assign rhs to lhs";
			}, rhs);
		}

		virtual ReflectedTypes get_mega_variant(void* obj) const override
		{
			obj;
			if constexpr (is_variant_member_v<std::decay_t<T>, ReflectedTypes>)
				return *static_cast<T*>(obj);
			else
				throw "Not part of idk::reflect::ReflectedTypes!";
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

		uni_container to_container(void* obj) const override
		{
			obj;
			if constexpr (is_sequential_container_v<T> || is_associative_container_v<T>)
				return uni_container{ *static_cast<T*>(obj) };
			else
				throw "not a container!";
		}

		enum_value to_enum_value(void* obj) const override
		{
			obj;
			if constexpr (is_macro_enum_v<T>)
				return get_type<T>().as_enum_type().from_value(*static_cast<T*>(obj));
			else
				throw "not an enum!";
		}

		template<size_t... Is>
		void unpack_helper(vector<dynamic>& vec, void* obj, std::index_sequence<Is...>) const
		{
			(vec.push_back(std::get<Is>(*static_cast<T*>(obj))), ...);
		}

		vector<dynamic> unpack(void* obj) const override
		{
			obj;
			if constexpr (is_template_v<T, std::pair> || is_template_v<T, std::tuple>)
			{
				vector<dynamic> vec;
				unpack_helper(vec, obj, std::make_index_sequence<std::tuple_size<T>::value>());
				return vec;
			}
			else
				throw "not a tuple!";
		}

		template<typename... Ts>
		dynamic get_variant_value(variant<Ts...>& var) const
		{
#define VARIANT_INDEX(I) case I: return std::get<I>(var);
#define VARIANT_CASE(N, ...) if constexpr (sizeof...(Ts) == N) { switch (var.index()) { IDENTITY(FOREACH(VARIANT_INDEX, __VA_ARGS__)) default: throw "Unhandled case?"; } }
					 VARIANT_CASE(1, 0)
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
		}

		virtual dynamic get_variant_value(void* obj) const override
		{
			obj;
			if constexpr (is_template_v<T, std::variant>)
				return get_variant_value(*static_cast<T*>(obj));
			else
				throw "not a variant!";
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