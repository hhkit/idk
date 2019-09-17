#pragma once
#include <idk.h>
#include <res/FileResources.h>

namespace idk
{
	class ExtensionLoader
	{
	public:
		virtual FileResources Create(PathHandle path_to_resource) = 0;
		virtual FileResources Create(PathHandle path_to_resource, const MetaFile& path_to_meta) = 0;
		//virtual bool Save(string_view path_to_resource) = 0;
		virtual ~ExtensionLoader() = default;
	};
}