#include "ResourceManager.h"
#include <res/ResourceMeta.h>
#pragma once

namespace idk
{
	template<typename Factory, typename ...Args>
	Factory& ResourceManager::RegisterFactory(Args&& ... args)
	{
		auto ptr = std::make_shared<Factory>(std::forward<Args>(args)...);
		_plaintext_loaders[ResourceID<typename Factory::Resource>] = ptr;
		_default_resources[ResourceID<typename Factory::Resource>] = ptr->Create();
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
		auto& table = GetTable<Resource>();
		auto ptr = GetLoader<Resource>().Create();
		auto handle = RscHandle<Resource>{ Guid::Make() };
		ptr->_handle = handle;
		table.emplace(handle.guid, std::move(ptr));
		return handle;
	}
	template<typename Resource>
	inline RscHandle<Resource> ResourceManager::Create(FileHandle filepath)
	{
		return Create<Resource>(filepath, Guid::Make());
	}

	template<typename Resource>
	inline RscHandle<Resource> ResourceManager::Create(FileHandle filepath, Guid guid)
	{
		auto [table, itr] = FindHandle(RscHandle<Resource>{guid});
		if (itr != table.end())
			return RscHandle<Resource>{};

		auto ptr = GetLoader<Resource>().Create(filepath);
		if (!ptr)
			return RscHandle<Resource>{};

		auto handle = RscHandle<Resource>{ guid };
		ptr->_handle = handle;
		table.emplace_hint(itr, handle.guid, std::move(ptr));
		return handle;
	}

	template<typename Resource, typename>
	inline RscHandle<Resource> ResourceManager::Create(FileHandle filepath, Guid guid, const typename Resource::Meta& meta)
	{
		auto [table, itr] = FindHandle(RscHandle<Resource>{guid});
		if (itr != table.end())
			return RscHandle<Resource>{};

		auto ptr = GetLoader<Resource>().Create(filepath, meta);
		if (!ptr)
			return RscHandle<Resource>{};

		auto handle = RscHandle<Resource>{ guid };
		ptr->_handle = handle;
		table.emplace_hint(itr, handle.guid, std::move(ptr));
		return handle;
	}

	template<typename Resource>
	inline auto& ResourceManager::GetLoader()
	{
		return *r_cast<ResourceFactory<Resource>*>(_plaintext_loaders[ResourceID<Resource>].get());
	}

	template<typename Resource>
	inline auto& ResourceManager::GetTable()
	{
		return *r_cast<Storage<Resource>*>(_resource_tables[ResourceID<Resource>].get());
	}

	template<typename Resource>
	auto ResourceManager::FindHandle(const RscHandle<Resource>& handle)
	{
		auto& table = GetTable<Resource>();
		return std::tuple<Storage<Resource>&, Storage<Resource>::iterator>{table, table.lower_bound(handle.guid)};
	}

	template<typename Resource>
	inline bool ResourceManager::Validate(const RscHandle<Resource>& handle)
	{
		auto [table, itr] = FindHandle(handle);
		return itr != table.end() && itr->second->_loaded;
	}

	template<typename Resource>
	inline Resource& ResourceManager::Get(const RscHandle<Resource>& handle)
	{
		auto [table, itr] = FindHandle(handle);
		if (itr != table.end() && itr->second->_loaded)
			return *itr->second;
		else
			return *r_cast<Resource*>(_default_resources[ResourceID<Resource>].get());
	}
	template<typename Resource>
	inline bool ResourceManager::Free(const RscHandle<Resource>& handle)
	{
		auto [table, itr] = FindHandle(handle);
		if (itr == table.end())
			return false;

		table.erase(itr);
		return true;
	}
}