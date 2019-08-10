#include "stdafx.h"
#include <reflect/reflect.h>

namespace idk::reflect
{

	type::type(const detail::typed_context_base* context)
		: _context{ context }
	{}

	bool type::valid() const
	{
		return _context;
	}

	string_view type::name() const
	{
		return _context->name;
	}

	size_t type::hash() const
	{
		return _context->hash;
	}

	bool type::operator==(type other) const
	{
		return hash() == other.hash();
	}

}