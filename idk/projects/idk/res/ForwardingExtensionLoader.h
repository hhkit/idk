#pragma once
#include <idk.h>
#include <res/ExtensionLoader.h>

namespace idk
{
	// forwards to the resource loader
	template<typename Resource>
	class ForwardingExtensionLoader final
		: public ExtensionLoader
	{
		FileResources Create(PathHandle path_to_resource) override;
		FileResources Create(PathHandle path_to_resource, const MetaFile& metadatas) override;
	};
}

#include "ForwardingExtensionLoader.inl"