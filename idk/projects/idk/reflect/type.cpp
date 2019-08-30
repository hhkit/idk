#include "stdafx.h"
#include <reflect/reflect.h>

namespace idk::reflect
{

	type::type(detail::typed_context_base* context)
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

	bool type::is_enum_type() const
	{
		return _context->is_enum_type;
	}

    bool type::is_basic_serializable() const
    {
        return _context->is_basic_serializable;
    }

	enum_type type::as_enum_type() const
	{
		return enum_type{ _context->get_enum_data() };
	}

	bool type::operator==(type other) const
	{
		return hash() == other.hash();
	}

}