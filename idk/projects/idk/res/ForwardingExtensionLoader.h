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
		FileResources Create(string_view path_to_resource) override;
		FileResources Create(string_view path_to_resource, string_view path_to_meta) override;
	};
}

#include "ForwardingExtensionLoader.inl"