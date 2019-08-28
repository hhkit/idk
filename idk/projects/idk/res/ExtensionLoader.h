#pragma once
#include <idk.h>
#include <res/FileResources.h>

namespace idk
{
	class ExtensionLoader
	{
	public:
		virtual FileResources Create(FileHandle path_to_resource) = 0;
		virtual FileResources Create(FileHandle path_to_resource, span<SerializedResourceMeta> path_to_meta) = 0;
		//virtual bool Save(string_view path_to_resource) = 0;
		virtual ~ExtensionLoader() = default;
	};
}