#include "ResourceManager.h"
#include <res/ResourceMeta.h>
#pragma once

namespace idk
{
	template<typename Factory, typename ...Args>
	Factory& ResourceManager::RegisterFactory(Args&& ... args)
	{
		auto ptr = std::make_shared<Factory>(std::forward<Args>(args)...);
		_plaintext_loaders[resource_ind<typename Factory::Resource>] = ptr;
		_default_resources[resource_ind<typename Factory::Resource>] = ptr->Create();
		return *ptr;
	}
	template<typename ExtensionLoaderT, typename ... Args>
	inline ExtensionLoaderT& ResourceManager::RegisterExtensionLoader(std::string_view extension, Args&& ... args)
	{
		static_assert(std::is_base_of<ExtensionLoader, ExtensionLoaderT>, "can only register extension loaders");
		return *(_extension_loaders[extension] = std::make_unique<ExtensionLoaderT>(std::forward<Args>(args)...));
	}
	template<typename Resource>
	inline RscHandle<Resource> ResourceManager::Create()
	{
		auto& table = *r_cast<Storage<Resource>*>(_resource_tables[resource_ind<Resource>].get());
		auto ptr = r_cast<ResourceFactory<Resource>*>(_plaintext_loaders[resource_ind<Resource>].get())->Create();
		auto handle = RscHandle<Resource>{ Guid::Make() };
		ptr->_handle = handle;
		table.emplace(handle.guid, std::move(ptr));
		return handle;
	}
	template<typename Resource>
	inline RscHandle<Resource> ResourceManager::Create(string_view filepath)
	{
		return Create<Resource>(filepath, Guid::Make());
	}
	template<typename Resource>
	inline RscHandle<Resource> ResourceManager::Create(string_view filepath, Guid guid)
	{
		auto& table = *r_cast<Storage<Resource>*>(_resource_tables[resource_ind<Resource>].get());
		auto itr = table.find(guid);
		if (itr != table.end())
			return RscHandle<Resource>{};

		auto ptr = r_cast<ResourceFactory<Resource>*>(_plaintext_loaders[resource_ind<Resource>].get())->Create(filepath);
		if (!ptr)
			return RscHandle<Resource>{};

		auto handle = RscHandle<Resource>{ guid };
		ptr->_handle = handle;
		table.emplace_hint(itr, handle.guid, std::move(ptr));
		return handle;
	}

	template<typename Resource>
	inline bool ResourceManager::Validate(const RscHandle<Resource>& handle)
	{
		auto& table = *r_cast<Storage<Resource>*>(_resource_tables[resource_ind<Resource>].get());
		auto itr = table.find(handle.guid); 
		return itr != table.end() && itr->second->_loaded;
	}

	template<typename Resource>
	inline Resource& ResourceManager::Get(const RscHandle<Resource>& handle)
	{
		auto& table = *r_cast<Storage<Resource>*>(_resource_tables[resource_ind<Resource>].get());
		if (auto itr = table.find(handle.guid); itr != table.end() && itr->second->_loaded)
			return *itr->second;
		else
			return *r_cast<Resource*>(_default_resources[resource_ind<Resource>].get());
	}
}