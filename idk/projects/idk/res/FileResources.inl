#pragma once

namespace idk
{
	template<typename Resource>
	RscHandle<Resource> GenericRscHandle::As() const
	{
		if (_handle.index() == index_in_tuple_v<Resource, Resources>)
			return std::get<RscHandle<Resource>>(_handle);
		else
			return RscHandle<Resource>();
	}
}