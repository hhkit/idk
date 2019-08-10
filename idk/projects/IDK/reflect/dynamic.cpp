#include "stdafx.h"
#include <reflect/reflect.h>

namespace idk::reflect
{
	dynamic::dynamic(reflect::type type, void* obj)
		: type{ type }, _ptr{ new detail::dynamic_derived<void*>(obj) }
	{}

	dynamic::dynamic()
		: type{ nullptr }, _ptr{ nullptr }
	{}

	dynamic& dynamic::operator=(const dynamic& rhs)
	{
		assert(rhs.type == type);
		type._context->copy_assign(_ptr->get(), rhs._ptr->get());
		return *this;
	}

	bool dynamic::valid() const
	{
		return type.valid();
	}
}