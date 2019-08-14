#pragma once

#include <reflect/reflect.h>

namespace idk::reflect
{

	template<typename... Ts>
	dynamic type::create(Ts&& ... args) const
	{
		return _context->construct(dynamic{ std::forward<Ts>(args) }...);
	}

	template<typename T>
	bool type::is() const
	{
		return hash() == typehash<T>();
	}

}