#include "stdafx.h"
#include "reflect.h"

namespace idk::reflect
{

	type get_type(string_view name)
	{
		auto iter = detail::meta::instance().names_to_contexts.find(name);
		return iter != detail::meta::instance().names_to_contexts.end() ? type{ iter->second } : type();
	}

}
