#pragma once
#include <idk_config.h>
#include "ResourceFile_detail.h"

namespace idk
{
	using GenericRscHandle = detail::ResourceFile_helper<Resources>::GenericRscHandle;

	struct ResourceFile
	{
		vector<GenericRscHandle> resources;
	};
}