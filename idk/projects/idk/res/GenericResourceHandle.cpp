#include "stdafx.h"

#include "GenericResourceHandle.h"

namespace idk
{
	Guid GenericResourceHandle::guid() const
	{
		return std::visit([](const auto& handle) { return handle.guid; }, *this);
	}

	size_t GenericResourceHandle::resource_id() const
	{
		return std::visit([](const auto& handle) 
			{
				return BaseResourceID<typename std::decay_t<decltype(handle)>::Resource>; 
			}, *this);
	}
}
