#pragma once

#include <idk_reflect_types.h>
#include <meta/meta.h>
#include <reflect/pretty_function.h>
#include <reflect/constructor_entry.h>
#include <util/string_hash.h>
#include <util/macro_utils.h>

// required settings for LIONant/properties
namespace property::settings
{
	using data_variant = idk::reflect::ReflectedTypes;
	struct user_entry { constexpr user_entry() = default; };
}
#include <properties/Properties.h>

namespace idk::reflect
{

	class type;
	class dynamic;
	struct property;
	class uni_container;
	class enum_type;
	class enum_value;
	//    ^^^ forward decls, but also bookmarks


	// get full qualified type name of T (decayed).
	// eg. vec3& => idk::math::vector<float, 3>
	// NOTE: if comparing types, use typehash<T>() !!!
	template<typename T> constexpr string_view fully_qualified_nameof() { return detail::pretty_function_name<std::decay_t<T>>(); }
	template<template<typename... > typename Tpl> constexpr string_view fully_qualified_nameof() { return detail::pretty_function_name<Tpl>(); }
	template<template<typename, auto> typename Tpl> constexpr string_view fully_qualified_nameof() { return detail::pretty_function_name<Tpl>(); }
	template<template<auto... > typename Tpl> constexpr string_view fully_qualified_nameof() { return detail::pretty_function_name<Tpl>(); }

	// gets hash of type T (decayed).
	// use this against type.hash()
	template<typename T> constexpr size_t typehash() { return idk::string_hash(fully_qualified_nameof<T>()); }

	// get type info with name
	type get_type(string_view name);

	// get type info of T
	template<typename T> type get_type();

	// see reflect.inl for detailed comments
	template<typename T, typename Visitor>
	void visit(T& obj, Visitor&& visitor);



	namespace detail
	{
		using table = ::property::table;
		template<typename T> using type_definition = ::property::opin::def<T>;

		struct meta
		{
			hash_table<string_view, struct typed_context_base*> names_to_contexts;
			hash_table<size_t, struct typed_context_base*> hashes_to_contexts;
			static meta& instance() { static meta s; return s; }
		};

		template<typename T, bool HasTypeDefinition> struct register_type { register_type(); };

		template<typename T> struct class_holder {};
		template<typename ClassT, typename... Ts> struct table_storage;

		struct typed_context_base;
		template<typename T> struct typed_context;

		template<typename Visitor>
		void visit(void* obj, type type, Visitor&& visitor, int& depth, int& last_visit_depth);
		template<typename K, typename V, typename Visitor>
		void visit_key_value(K&& key, V&& val, Visitor&& visitor, int& depth, int& curr_depth);
	}



	// type class, contains info about reflected type.
	// use idk::reflect::get_type to obtain type.
	class type
	{
	public:
		// construct an instance of this type
		template<typename... Ts>
		dynamic create(Ts&& ... args) const;

		// gets the alias if reflected, otherwise fully qualified type name ( fully_qualified_nameof<T>() )
		string_view name() const;

		// gets the hash of the type ( check against typehash<T>() )
		size_t hash() const;

		// should always be true for now (since get_type has assert)
		bool valid() const;

		// number of properties
		size_t count() const;

		template<typename T>
		bool is() const;

		// is it a container type?
		bool is_container() const;

		// is it a smart enum type defined with the macro ENUM?
		bool is_enum_type() const;

		// Checks if this type is a template type Tpl<typename...>
		template<template<typename...> typename Tpl> bool is_template() const;
		// Checks if this type is a template type Tpl<typename, auto>
		template<template<typename, auto> typename Tpl> bool is_template() const;
		// Checks if this type is a template type Tpl<auto...>
		template<template<auto...> typename Tpl> bool is_template() const;

		enum_type as_enum_type() const;

		bool operator==(type other) const;

	private:
		const detail::typed_context_base* _context;
		explicit type(detail::typed_context_base* context = nullptr);

		friend class dynamic;
		friend struct detail::typed_context_base;
		friend type get_type(string_view name);
		template<typename T> friend type get_type();
		template<typename Visitor> friend void detail::visit(void* obj, type type, Visitor&& visitor, int& depth, int& last_visit_depth);
	};



	// reflected object. contains type-erased object with type information.
	class dynamic
	{
	public:
		class property_iterator;
		const type type;

		template<typename T, typename = std::enable_if_t<!std::is_same_v<std::decay_t<T>, dynamic>>>
		dynamic(T&& obj);

		// invalid dynamic
		dynamic();

		template<typename T> bool is() const;
		template<typename T> T& get() const;
		bool valid() const;

		// see reflect.inl for detailed comments
		template<typename Visitor>
		void visit(Visitor&& visitor) const;

		// iterate properties. to iterate container types, use to_container()
		property_iterator begin() const;
		property_iterator end() const;

		// get property with name
		property get_property(string_view name) const;
		// get property with index (arg index in REFLECT_VARS)
		property get_property(size_t index) const;

		// convert to unified container. check using type.is_container()
		uni_container to_container() const;

		// convert to a smart enum value. check using type.is_enum_type()
		enum_value to_enum_value() const;

		// unpacks a tuple
		vector<dynamic> unpack() const;

		// if this is a variant, gets the held variant value. check using type.is_template<std::variant>()
		dynamic get_variant_value() const;

		template<typename T, typename = std::enable_if_t<!std::is_same_v<std::decay_t<T>, dynamic>>>
		dynamic& operator=(T&& rhs);
		dynamic& operator=(const dynamic& rhs);

	private:
		struct base;
		template<typename T> struct derived;

		shared_ptr<base> _ptr;
		dynamic(reflect::type type, void* obj);

		friend struct detail::typed_context_base;
		template<typename Visitor> friend void detail::visit(void* obj, reflect::type type, Visitor&& visitor, int& depth, int& last_visit_depth);
		template<typename K, typename V, typename Visitor> friend void detail::visit_key_value(K&& key, V&& val, Visitor&& visitor, int& depth, int& last_visit_depth);
	};




	// represents a reflected type property.
	struct property
	{
		const string_view name;
		const dynamic value;
	};



	// a unified container class. type-erases container and gives a generic interface for containers.
	class uni_container
	{
	public:
		class iterator;
		const type value_type;
		const type type;

		template<typename T, typename = std::enable_if_t<
			!std::is_same_v<std::decay_t<T>, uni_container> &&
			!std::is_same_v<std::decay_t<T>, dynamic> &&
			is_iterable_v<std::decay_t<T>>>>
		uni_container(T&& obj);

		size_t size() const;

		// generic iterators which dereferences into value_type as a dynamic
		iterator begin() const;
		iterator end() const;

		// push_back for sequential containers, insert for associative,
		// throws if container supports neither
		void add(const dynamic& elem);

		// clears the container. throws if container doesn't support clear.
		void clear();

	private:
		struct base;
		template<typename T> struct derived;

		std::shared_ptr<base> _ptr;
	};



	// represents a smart enum built using the macro ENUM
	class enum_type
	{
	public:
		class iterator;

		type underlying_type() const;

		// constructs an enum_value from name. if invalid name, uses the first enumerator.
		enum_value from_string(string_view name) const;

		// constructs an enum_value from value. the value may be invalid.
		enum_value from_value(int64_t value) const;

		// the number of enumerators.
		size_t count() const;

		// iterate name-value pairs.
		iterator begin() const;
		iterator end() const;

		bool operator==(const enum_type& other) const;

	private:
		struct data { const size_t value_sizeof; const size_t count; const void* values; const string_view* names; };

		const data* _data;
		explicit enum_type(const data* data);
		int64_t at(size_t index) const;

		friend struct detail::typed_context_base;
		template<typename T> friend struct detail::typed_context;
		friend class type;
		friend class enum_value;
	};

	// represents a smart enum value, see enum_type
	class enum_value
	{
	public:
		constexpr static size_t npos = static_cast<size_t>(-1);
		const enum_type enum_type;

		template<typename T, typename = std::enable_if_t<is_macro_enum_v<T>>>
		enum_value(T t) : enum_value(get_type<T>().as_enum_type(), t) {}

		// get the name. if invalid value, returns empty string ""
		string_view name() const;
		// get the value casted to long long.
		int64_t value() const;
		// get the index of the value, ie position declared in the enum.
		// if invalid value, returns enum_value::npos
		size_t index() const;

		// assigns value if name exists, does nothing otherwise.
		enum_value& try_assign(string_view name);
		// assigns value if index exists, does nothing otherwise.
		enum_value& try_assign(size_t index);
		// assigns value if it is valid, does nothing otherwise.
		enum_value& try_assign(int64_t val);
		// unsafe assignment, works even if value is invalid
		enum_value& assign(int64_t val);

		bool operator==(const enum_value& other) const;

	private:
		int64_t _value;
		explicit enum_value(const reflect::enum_type type, int64_t val);

		friend class enum_type;
	};

};

#include "reflect.inl"



#define REFLECT_BEGIN(CLASS_TYPE, ALIAS)		template<> struct property::opin::def<CLASS_TYPE>{ using t_self = CLASS_TYPE; constexpr static char m_Name[] = ALIAS; inline static const idk::reflect::detail::table_storage m_Storage { idk::reflect::detail::class_holder<t_self>{},
#define REFLECT_END()							}; inline static const property::table_hash<m_Storage.entry_count_v> m_Table{ m_Storage, m_Name }; inline static const idk::reflect::detail::register_type<t_self, true> __reg{}; };
#define REFLECT_VAR(VAR)						property::PropertyVar<decltype(std::declval<t_self>().VAR)>( #VAR, offsetof( t_self, VAR ) )
#define REFLECT_PARENT(PARENT_TYPE)				property::PropertyParent<t_self::PARENT_TYPE, const t_self*>(),
#define REFLECT_CTOR(...)						idk::reflect::detail::constructor_entry<t_self, __VA_ARGS__>{},
#define REFLECT_FRIEND							template<typename> friend struct property::opin::def;

#define X_REFLECT_VARS_SINGLE(VAR)				REFLECT_VAR(VAR),
#define REFLECT_VARS(...)						IDENTITY(FOREACH(X_REFLECT_VARS_SINGLE, __VA_ARGS__))

#define REFLECT_ENUM(ENUM_TYPE, ALIAS)			REFLECT_BEGIN(ENUM_TYPE, ALIAS) REFLECT_VARS(value) REFLECT_CTOR(ENUM_TYPE::_enum) REFLECT_CTOR(int8_t) REFLECT_CTOR(int16_t) REFLECT_CTOR(int32_t) REFLECT_CTOR(int64_t) REFLECT_END()

#undef property_var
#undef property_var_fnbegin
#undef property_var_fnend
#undef property_list_fnbegin
#undef property_list_fnenum
#undef property_list_fnend
#undef property_scope_begin
#undef property_scope_end
#undef property_parent
#undef property_begin_name
#undef property_begin
#undef property_end
#undef property_vend_h
#undef property_vend_cpp
#undef property_friend
#undef property_vtable
