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
	inline FileResources ForwardingExtensionLoader<T>::Create(FileHandle path_to_resource, const MetaFile& metadatas)
	{
		if (metadatas.guids.empty())
			return Create(path_to_resource);

		FileResources retval;
		if (metadatas.guids.size())
		{
			if constexpr (has_tag_v<T, MetaTag>)
			{
				using Metadata = typename std::decay_t<T>::Metadata;
				if (metadatas.resource_metas.size())
				{
					auto& meta = metadatas.resource_metas[0];
					if (meta.valid() && meta.is<Metadata>())
					{
						retval.resources.emplace_back(Core::GetResourceManager().Create<T>(path_to_resource, metadatas.guids[0], meta.get<Metadata>()));
						return retval;
					}
				}
			}
			retval.resources.emplace_back(Core::GetResourceManager().Create<T>(path_to_resource, metadatas.guids[0]));
			return retval;
		}

		return retval;
	}
}