#pragma once
#include <core/Core.h>
#include <res/ResourceManager.h>

namespace idk 
{
	template<typename T>
	inline FileResources ForwardingExtensionLoader<T>::Create(string_view path_to_resource)
	{
		return Core::GetResourceManager().Create<T>(path_to_resource);
	}

	template<typename T>
	inline FileResources ForwardingExtensionLoader<T>::Create(string_view path_to_resource, string_view path_to_meta)
	{
		return Core::GetResourceManager().Create<T>(path_to_resource);
	}
}