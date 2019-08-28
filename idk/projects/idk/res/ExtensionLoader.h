#pragma once
#include <idk.h>
#include <res/FileResources.h>

namespace idk
{
	class ExtensionLoader
	{
	public:
		virtual FileResources Create(string_view path_to_resource) = 0;
		virtual FileResources Create(string_view path_to_resource, string_view path_to_meta) = 0;
		//virtual bool Save(string_view path_to_resource) = 0;
		virtual ~ExtensionLoader() = default;
	};
}