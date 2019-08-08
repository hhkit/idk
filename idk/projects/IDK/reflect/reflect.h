#pragma once

#include <idk.h>
#include <idk_config.h>

namespace property::settings
{
	using data_variant = idk::ReflectedTypes;
	struct user_entry { constexpr user_entry() = default; };
}
#include <properties/Properties.h>

namespace idk::reflect
{
	void t()
	{
		vec3 test;
		property::DisplayEnum(test, [&](std::string_view PropertyName, property::data&& Data, const property::table&, std::size_t, property::flags::type Flag)
		{
			auto name = PropertyName;
		});
	}
};

//#define REFLECT_VAR_FNBEGIN(NAME, TYPE)		property_var_fnbegin(NAME, TYPE)
//#define REFLECT_VAR_FNEND()					property_var_fnend()
//#define REFLECT_LIST_FNBEGIN(NAME, TYPE)	property_list_fnbegin(NAME, TYPE)
//#define REFLECT_LIST_FNENUM()				property_list_fnenum()
//#define REFLECT_LIST_FNEND()				property_list_fnend()
//#define REFLECT_SCOPE_BEGIN(NAME)			property_scope_begin(NAME)
//#define REFLECT_SCOPE_END()					property_scope_end()
#define REFLECT_BEGIN_ALIAS(CLASS_TYPE, ALIAS)	template<> struct property::opin::def<CLASS_TYPE>{ using t_self = CLASS_TYPE; constexpr static char m_Name[] = ALIAS;        vs2017_hack_inline static const table_storage m_Storage {
#define REFLECT_BEGIN(CLASS_TYPE)				REFLECT_BEGIN_ALIAS(CLASS_TYPE, #CLASS_TYPE)
#define REFLECT_END()							}; vs2017_hack_inline static const property::table_hash<m_Storage.entry_count_v> m_Table{ m_Storage, m_Name }; };
#define REFLECT_VAR(VAR)						property::PropertyVar<decltype(std::declval<t_self>().VAR)>( #VAR, offsetof( t_self, VAR ) )
#define REFLECT_PARENT(PARENT_TYPE)				property::PropertyParent<t_self::PARENT_TYPE, const t_self*>()
#define REFLECT_FRIEND							template<typename> friend struct property::opin::def;

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

#include <reflect/macro_utils.h>

#define X_REFLECT_VARS_SINGLE(VAR) REFLECT_VAR(VAR),
#define REFLECT_VARS(...) IDENTITY(FOREACH(X_REFLECT_VARS_SINGLE, __VA_ARGS__))

REFLECT_BEGIN_ALIAS(idk::vec3, "vec3")
	REFLECT_VARS(x, y, z)
REFLECT_END()