#pragma once
#include <core/Core.h>
#include <res/ResourceManager.h>

namespace idk 
{
	template<typename T>
	inline FileResources ForwardingExtensionLoader<T>::Create(FileHandle path_to_resource)
	{
		return Core::GetResourceManager().Create<T>(path_to_resource);
	}

	template<typename T>
	inline FileResources ForwardingExtensionLoader<T>::Create(FileHandle path_to_resource, span<SerializedResourceMeta> metadatas)
	{
		assert(metadatas.size() == 1);
		if constexpr(has_tag_v<T, MetaTag>)
			Core::GetResourceManager().Create<T>(path_to_resource, metadatas[0].guid, metadatas[0].metadata.get<typename T::Meta>())
		else
			return Core::GetResourceManager().Create<T>(path_to_resource, metadatas[0].guid);
	}
}