#pragma once
#include <idk.h>
#include <res/ExtensionLoader.h>

namespace idk
{
	class OpenGLFBXLoader
		: public ExtensionLoader
	{
	public:
		FileResources Create(FileHandle path_to_resource) override;
		FileResources Create(FileHandle path_to_resource, span<SerializedResourceMeta> path_to_meta) override;
	};
}