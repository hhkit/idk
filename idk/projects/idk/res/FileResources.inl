#include "FileResources.h"
#include <reflect/reflect.h>
#pragma once

namespace idk
{
	template<typename Resource>
	inline GenericRscHandle::GenericRscHandle(const RscHandle<Resource>& rhs)
		: _handle{rhs}
	{
	}
	template<typename Resource>
	RscHandle<Resource> GenericRscHandle::As() const
	{
		if (_handle.index() == index_in_tuple_v<Resource, Resources>)
			return std::get<RscHandle<Resource>>(_handle);
		else
			return RscHandle<Resource>();
	}
	template<typename Func>
	inline auto GenericRscHandle::visit(Func&& func)
	{
		return std::visit(func, _handle);
	}

	template<typename Func>
	inline auto GenericRscHandle::visit(Func&& func) const
	{
		return std::visit(func, _handle);
	}
}