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

	size_t type::count() const
	{
		return _context->table.m_Count;
	}

	bool type::is_container() const
	{
		return _context->is_container;
	}

	bool type::operator==(type other) const
	{
		return hash() == other.hash();
	}

}