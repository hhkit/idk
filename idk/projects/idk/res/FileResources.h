#pragma once
#include <idk_config.h>
#include <reflect/reflect.h>
#include "FileResources_detail.h"

namespace idk
{
	struct SerializedResourceMeta;
	struct FileResources;

	class GenericRscHandle
	{
	public:
		template<typename Resource>
		RscHandle<Resource> As() const;
		explicit operator bool();
	private:
		detail::ResourceFile_helper<Resources>::GenericRscHandle _handle;
		friend class ResourceManager;
		friend vector<SerializedResourceMeta> serialize(const FileResources& resources);
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

	vector<SerializedResourceMeta> serialize(const FileResources&);
}

#include "FileResources.inl"