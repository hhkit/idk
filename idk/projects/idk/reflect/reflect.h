#pragma once

#include <idk_reflect_types.h>
#include <meta/meta.h>
#include <reflect/pretty_function.h>
#include <reflect/constructor_entry.h>
#include <reflect/dynamic_detail.h>
#include <util/string_hash.h>

// required settings for LIONant/properties
namespace property::settings
{
	using data_variant = idk::reflect::ReflectedTypes;
	struct user_entry { constexpr user_entry() = default; };
}
#include <properties/Properties.h>

namespace idk::reflect
{
	// get full qualified type name of T (decayed).
	// eg. vec3& => idk::math::vec3
	template<typename T> constexpr string_view nameof() { return detail::pretty_function_name<std::decay_t<T>>(); }

	// gets hash of type T (decayed).
	// use this against type.hash()
	template<typename T> constexpr size_t typehash() { return idk::string_hash(nameof<T>()); }

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

		template<typename T> struct register_type { register_type(); };

		template<typename T> struct class_holder {};
		template<typename ClassT, typename... Ts> struct table_storage;

		struct typed_context_base;
		template<typename T> struct typed_context;

		template<typename Visitor>
		void visit(void* obj, const detail::table& table, Visitor&& visitor);
	}



	// type class, contains info about reflected type.
	// use idk::reflect::get_type to obtain type.
	class type
	{
		friend class dynamic;
		friend struct detail::typed_context_base;
		friend type get_type(string_view name);
		template<typename T> friend type get_type();
		template<typename T, typename Visitor> friend void visit(T& obj, Visitor&& visitor);

	public:
		template<typename... Ts>
		dynamic create(Ts&& ... args) const;

		// gets the alias if reflected, otherwise full qualified type name ( nameof<T>() )
		string_view name() const;

		// gets the hash of the type ( check against typehash<T>() )
		size_t hash() const;

		// should always be true for now (since get_type has assert)
		bool valid() const;

		bool operator==(type other) const;

	private:
		const detail::typed_context_base* _context;
		type(const detail::typed_context_base* context = nullptr);
	};



	struct property;

	// reflected object. contains type-erased object with type information.
	class dynamic
	{
		struct property_iterator;

	public:
		const type type;

		template<typename T, typename = std::enable_if_t<!std::is_same_v<std::decay_t<T>, dynamic>>>
		dynamic(T&& obj);

		// invalid dynamic
		dynamic();

		dynamic& operator=(const dynamic& rhs);

		template<typename T, typename = std::enable_if_t<!std::is_same_v<std::decay_t<T>, dynamic>>>
		dynamic& operator=(const T& rhs);

		template<typename T> bool is() const;
		template<typename T> T& get() const;

		bool valid() const;

		// recursively visit all members
		// visitor must be a function with signature:
		//    (const char* name, auto&& data) -> bool/void
		// return false to stop recursion. if function doesn't return, it always recurses
		template<typename Visitor>
		void visit(Visitor&& visitor);

		property_iterator begin() const;
		property_iterator end() const;
		property get_property(string_view name) const;
		property get_property(size_t index) const;

	private:
		shared_ptr<detail::dynamic_base> _ptr;
		dynamic(reflect::type type, void* obj);
	};



	struct property
	{
		const string_view name;
		const dynamic value;
	};



	// get type info with name
	type get_type(string_view name);
	// get type info of T
	template<typename T> type get_type();

	// recursively visit all members of an object
	// visitor must be a function with signature:
	//    (const char* name, auto&& data) -> bool/void
	// return false to stop recursion. if function doesn't return, it always recurses
	template<typename T, typename Visitor>
	void visit(T& obj, Visitor&& visitor);

};

#include "reflect.inl"



//#define REFLECT_VAR_FNBEGIN(NAME, TYPE)		property_var_fnbegin(NAME, TYPE)
//#define REFLECT_VAR_FNEND()					property_var_fnend()
//#define REFLECT_LIST_FNBEGIN(NAME, TYPE)		property_list_fnbegin(NAME, TYPE)
//#define REFLECT_LIST_FNENUM()					property_list_fnenum()
//#define REFLECT_LIST_FNEND()					property_list_fnend()
//#define REFLECT_SCOPE_BEGIN(NAME)				property_scope_begin(NAME)
//#define REFLECT_SCOPE_END()					property_scope_end()
#define REFLECT_BEGIN_ALIAS(CLASS_TYPE, ALIAS)	template<> struct property::opin::def<CLASS_TYPE>{ using t_self = CLASS_TYPE; constexpr static char m_Name[] = ALIAS; inline static const idk::reflect::detail::table_storage m_Storage { idk::reflect::detail::class_holder<t_self>{},
#define REFLECT_BEGIN(CLASS_TYPE)				REFLECT_BEGIN_ALIAS(CLASS_TYPE, #CLASS_TYPE)
#define REFLECT_END()							}; inline static const property::table_hash<m_Storage.entry_count_v> m_Table{ m_Storage, m_Name }; inline static const idk::reflect::detail::register_type<t_self> __reg{}; };
#define REFLECT_VAR(VAR)						property::PropertyVar<decltype(std::declval<t_self>().VAR)>( #VAR, offsetof( t_self, VAR ) )
#define REFLECT_PARENT(PARENT_TYPE)				property::PropertyParent<t_self::PARENT_TYPE, const t_self*>(),
#define REFLECT_CTOR(...)						idk::reflect::detail::constructor_entry<t_self, __VA_ARGS__>{},
#define REFLECT_FRIEND							template<typename> friend struct idk::reflect::detail::type_definition;

#include <reflect/macro_utils.h>
#define X_REFLECT_VARS_SINGLE(VAR) REFLECT_VAR(VAR),
#define REFLECT_VARS(...) IDENTITY(FOREACH(X_REFLECT_VARS_SINGLE, __VA_ARGS__))

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



REFLECT_BEGIN_ALIAS(idk::vec2, "vec2")
	REFLECT_CTOR(float, float)
	REFLECT_VARS(x, y)
REFLECT_END()

REFLECT_BEGIN_ALIAS(idk::vec3, "vec3")
	REFLECT_CTOR(float, float, float)
	REFLECT_VARS(x, y, z)
REFLECT_END()

REFLECT_BEGIN_ALIAS(idk::vec4, "vec4")
	REFLECT_CTOR(float, float, float, float)
	REFLECT_VARS(x, y, z, w)
REFLECT_END()