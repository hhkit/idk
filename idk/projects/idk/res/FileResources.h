#pragma once
#include <idk_config.h>
#include <reflect/reflect.h>
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
		friend class ResourceManager;
	};

	struct FileResources
	{
		vector<GenericRscHandle> resources;
	};

	struct SerializedResourceMeta
	{
		Guid guid;
		reflect::dynamic metadata;
	};
}

#include "FileResources.inl"