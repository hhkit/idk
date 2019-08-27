#pragma once
#include <idk_config.h>
#include "FileResources_detail.h"

namespace idk
{
	class GenericRscHandle
	{
	public:
		template<typename Resource>
		RscHandle<Resource> As() const;
		explicit operator bool();
	private:
		detail::ResourceFile_helper<Resources>::GenericRscHandle _handle;
	};

	struct FileResources
	{
		vector<GenericRscHandle> resources;
	};
}

#include "FileResources.inl"