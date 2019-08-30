#pragma once
#include <core/Core.h>
#include <res/ResourceManager.h>

namespace idk 
{
	template<typename T>
	inline FileResources ForwardingExtensionLoader<T>::Create(FileHandle path_to_resource)
	{
		FileResources retval;
		auto newresource = Core::GetResourceManager().Create<T>(path_to_resource);
		if constexpr (has_tag_v<T, MetaTag>)
			newresource->_dirtymeta = true;
		retval.resources.emplace_back(newresource);
		return retval;
	}

	template<typename T>
	inline FileResources ForwardingExtensionLoader<T>::Create(FileHandle path_to_resource, span<GenericMetadata> metadatas)
	{
		assert(metadatas.size() == 1);
		FileResources retval;
		if (metadatas.size())
		{
			if constexpr (has_tag_v<T, MetaTag>)
			{
				auto meta = metadatas[0].GetMeta<T>();
				if (meta)
				{
					retval.resources.emplace_back(Core::GetResourceManager().Create<T>(path_to_resource, metadatas[0].guid, *meta));
					return retval;
				}
			}
			retval.resources.emplace_back(Core::GetResourceManager().Create<T>(path_to_resource, metadatas[0].guid));
			return retval;
			}

		return retval;
	}
}