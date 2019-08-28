#include "stdafx.h"
#include <IncludeResources.h>

namespace idk
{
	GenericRscHandle::operator bool()
	{
		return std::visit([](auto handle) { return static_cast<bool>(handle); }, _handle);
	}
}
